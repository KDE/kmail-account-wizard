/*
    Copyright (c) 2016  Daniel Vrátil <dvratil@kde.org>

    This library is free software; you can redistribute it and/or modify it
    under the terms of the GNU Library General Public License as published by
    the Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    This library is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
    License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to the
    Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
    02110-1301, USA.
*/

#include "key.h"
#include "setupmanager.h"
#include "transport.h"

#include <QTimer>
#include <QProcess>

#include <QGpgME/Protocol>
#include <QGpgME/WKSPublishJob>
#include <QGpgME/CryptoConfig>

#include <gpgme++/engineinfo.h>

#include <MailTransport/TransportManager>
#include <MailTransport/Transport>
#include <MailTransport/MessageQueueJob>

#include <KIdentityManagement/IdentityManager>
#include <KIdentityManagement/Identity>

#include <KMime/Message>
#include <KMime/Headers>
#include <KMime/Util>

#include <KCodecs/KEmailAddress>

#include <KLocalizedString>

#include "accountwizard_debug.h"

Key::Key(QObject *parent)
    : SetupObject(parent)
    , m_transportId(0)
    , m_publishingMethod(NoPublishing)
{
}

Key::~Key()
{
}

void Key::setKey(const GpgME::Key &key)
{
    m_key = key;
}

void Key::setMailBox(const QString &mailbox)
{
    m_mailbox = KEmailAddress::extractEmailAddress(mailbox);
}

void Key::setTransportId(int transportId)
{
    m_transportId = transportId;
}

void Key::setPublishingMethod(PublishingMethod method)
{
    m_publishingMethod = method;
}

void Key::create()
{
    switch (m_publishingMethod) {
    case NoPublishing:
        QTimer::singleShot(0, this, [this]() {
            Q_EMIT finished(i18n("Skipping key publishing"));
        });
        break;
    case WKS:
        publishWKS();
        break;
    case PKS:
        publishPKS();
        break;
    }
}

void Key::publishWKS()
{
    Q_EMIT info(i18n("Publishing OpenPGP key..."));

    auto job = QGpgME::openpgp()->wksPublishJob();
    mJob = job;
    connect(job, &QGpgME::WKSPublishJob::result,
            this, &Key::onWKSPublishingCheckDone);
    job->startCheck(m_mailbox);
}

void Key::onWKSPublishingCheckDone(const GpgME::Error &error,
                                   const QByteArray &,
                                   const QByteArray &returnedError)
{
    mJob = Q_NULLPTR;

    if (error) {
        if (error.isCanceled()) {
            Q_EMIT this->error(i18n("Key publishing was canceled."));
            return;
        }

        qCWarning(ACCOUNTWIZARD_LOG) << "Check error:" << returnedError;
        if (error.code() == GPG_ERR_NOT_SUPPORTED) {
            Q_EMIT info(i18n("Key publishing failed: not online, or GnuPG too old."));
            Q_EMIT finished(QString());
        } else {
            Q_EMIT info(i18n("Your email provider does not support key publishing."));
            Q_EMIT finished(QString());
        }
        return;
    }

    auto job = QGpgME::openpgp()->wksPublishJob();
    mJob = job;
    connect(job, &QGpgME::WKSPublishJob::result,
            this, &Key::onWKSPublishingRequestCreated);
    job->startCreate(m_key.primaryFingerprint(), m_mailbox);
}

void Key::onWKSPublishingRequestCreated(const GpgME::Error &error,
                                        const QByteArray &returnedData,
                                        const QByteArray &returnedError)
{
    mJob = Q_NULLPTR;

    if (error) {
        if (error.isCanceled()) {
            Q_EMIT this->error(i18n("Key publishing was canceled."));
            return;
        }

        qCWarning(ACCOUNTWIZARD_LOG) << "Publishing error:" << returnedData << returnedError;
        Q_EMIT this->error(i18n("An error occurred while creating key publishing request."));
        return;
    }

    if (m_transportId == 0 && qobject_cast<SetupManager *>(parent())) {
        const auto setupManager = qobject_cast<SetupManager *>(parent());
        const auto setupObjects = setupManager->setupObjects();
        auto it = std::find_if(setupObjects.cbegin(), setupObjects.cend(),
                               [](SetupObject * obj) -> bool { return qobject_cast<Transport *>(obj);});
        if (it != setupObjects.cend()) {
            m_transportId = qobject_cast<Transport *>(*it)->transportId();
        }
    } else if (m_transportId) {
        auto ident = KIdentityManagement::IdentityManager::self()->identityForAddress(m_mailbox);
        if (!ident.transport().isEmpty()) {
            m_transportId = ident.transport().toInt();
        }
    }

    auto transport = MailTransport::TransportManager::self()->transportById(m_transportId, true);
    if (!transport) {
        qCWarning(ACCOUNTWIZARD_LOG) << "No MailTransport::Transport available?!?!?!";
        Q_EMIT this->error(i18n("Key publishing error: mail transport is not configured"));
        return;
    }

    qCDebug(ACCOUNTWIZARD_LOG) << returnedData;

    // Parse the data so that we can get "To" and "From" headers
    auto msg = KMime::Message::Ptr::create();
    msg->setContent(KMime::CRLFtoLF(returnedData));
    msg->parse();

    if (!msg->from(false) || !msg->to(false)) {
        qCWarning(ACCOUNTWIZARD_LOG) << "No FROM or TO in parsed message, source data were:" << returnedData;
        Q_EMIT this->error(i18n("Key publishing error: failed to create request email"));
        return;
    }

    auto header = new KMime::Headers::Generic("X-KMail-Transport");
    header->fromUnicodeString(QString::number(m_transportId), "utf-8");
    msg->setHeader(header);

    // Build the message
    msg->assemble();

    // Move to outbox
    auto job = new MailTransport::MessageQueueJob;
    mJob = job;
    job->addressAttribute().setTo({ msg->to(false)->asUnicodeString() });
    job->transportAttribute().setTransportId(transport->id());
    job->addressAttribute().setFrom(msg->from(false)->asUnicodeString());
    // Don't leave any evidence :-)
    job->sentBehaviourAttribute().setSentBehaviour(MailTransport::SentBehaviourAttribute::Delete);
    job->sentBehaviourAttribute().setSendSilently(true);
    job->setMessage(msg);
    connect(job, &KJob::result,
            this, &Key::onWKSPublishingRequestSent);
    job->start();
}

void Key::onWKSPublishingRequestSent(KJob *job)
{
    mJob = Q_NULLPTR;
    if (job->error() == KJob::KilledJobError) {
        Q_EMIT error(i18n("Key publishing was canceled."));
    } else if (job->error()) {
        Q_EMIT error(i18n("Failed to send key publishing request: %1", job->errorString()));
    } else {
        Q_EMIT finished(i18n("Key publishing request sent."));
    }
}

void Key::publishPKS()
{
    Q_EMIT info(i18n("Publishing OpenPGP key..."));

    // default
    QString keyServer = QStringLiteral("key.gnupg.net");

    const auto config = QGpgME::cryptoConfig();
    if (config) {
        const auto entry = config->entry(QStringLiteral("gpg"), QStringLiteral("Keyserver"), QStringLiteral("keyserver"));
        if (entry && !entry->stringValue().isEmpty()) {
            keyServer = entry->stringValue();
        }
    }

    const char *gpgName = GpgME::engineInfo(GpgME::OpenPGP).fileName();
    auto gpgProcess = new QProcess;
    gpgProcess->setProperty("keyServer", keyServer);
    connect(gpgProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &Key::onPKSPublishingFinished);
    mJob = gpgProcess;
    gpgProcess->start(QString::fromLatin1(gpgName), {
        QStringLiteral("--keyserver"), keyServer,
        QStringLiteral("--send-keys"), QString::fromLatin1(m_key.primaryFingerprint())
    });
}

void Key::onPKSPublishingFinished(int code, QProcess::ExitStatus status)
{
    auto process = qobject_cast<QProcess *>(mJob);
    mJob = Q_NULLPTR;
    process->deleteLater();

    if (status != QProcess::NormalExit || code != 0) {
        qCWarning(ACCOUNTWIZARD_LOG) << "PKS Publishing error:" << process->readAll();
        Q_EMIT info(i18n("Failed to publish the key."));
        Q_EMIT finished(QString());
        return;
    }

    const auto keyServer = process->property("keyServer").toString();
    Q_EMIT finished(i18n("Key has been published on %1", keyServer));
}

void Key::destroy()
{
    // This is all we can do, there's no unpublish...
    if (auto job = qobject_cast<QGpgME::Job *>(mJob)) {
        job->slotCancel();
    } else if (auto job = qobject_cast<KJob *>(mJob)) {
        job->kill();
    } else if (auto job = qobject_cast<QProcess *>(mJob)) {
        job->kill();
    }
}
