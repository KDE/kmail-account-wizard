/*
    SPDX-FileCopyrightText: 2010-2022 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

// This code was taken from kmail-account-wizard

#include "identity.h"
#include "accountwizard_debug.h"

#include <KIdentityManagementCore/Identity>
#include <KIdentityManagementCore/IdentityManager>
#include <MailTransport/Transport>

#include <KLocalizedString>

Identity::Identity(QObject *parent)
    : QObject(parent)
{
    m_identity = &KIdentityManagementCore::IdentityManager::self()->newFromScratch(QString());
    Q_ASSERT(m_identity != nullptr);
}

void Identity::create()
{
    Q_EMIT info(i18n("Setting up identity..."));

    // store identity information
    m_identityName = identityName();
    m_identity->setIdentityName(m_identityName);
    auto manager = KIdentityManagementCore::IdentityManager::self();
    manager->commit();
    if (!manager->setAsDefault(m_identity->uoid())) {
        qCWarning(ACCOUNTWIZARD_LOG) << "Impossible to find identity";
    }

    Q_EMIT finished(i18n("Identity set up."));
}

QString Identity::identityName() const
{
    // create identity name
    QString name(m_identityName);
    if (name.isEmpty()) {
        name = i18nc("Default name for new email accounts/identities.", "Unnamed");

        const QString idName = m_identity->primaryEmailAddress();
        int pos = idName.indexOf(QLatin1Char('@'));
        if (pos != -1) {
            name = idName.mid(0, pos);
        }

        // Make the name a bit more human friendly
        name.replace(QLatin1Char('.'), QLatin1Char(' '));
        pos = name.indexOf(QLatin1Char(' '));
        if (pos != 0) {
            name[pos + 1] = name[pos + 1].toUpper();
        }
        name[0] = name[0].toUpper();
    }

    auto manager = KIdentityManagementCore::IdentityManager::self();
    if (!manager->isUnique(name)) {
        name = manager->makeUnique(name);
    }
    return name;
}

void Identity::destroy()
{
    auto manager = KIdentityManagementCore::IdentityManager::self();
    if (!manager->removeIdentityForced(m_identityName)) {
        qCWarning(ACCOUNTWIZARD_LOG) << " impossible to remove identity " << m_identityName;
    }
    manager->commit();
    m_identity = nullptr;
    Q_EMIT info(i18n("Identity removed."));
}

void Identity::setIdentityName(const QString &name)
{
    m_identityName = name;
}

QString Identity::fullName() const
{
    return m_identity->fullName();
}

void Identity::setFullName(const QString &name)
{
    if (name == fullName()) {
        return;
    }
    m_identity->setFullName(name);
    Q_EMIT fullNameChanged();
}

QString Identity::organization() const
{
    return m_identity->organization();
}

void Identity::setOrganization(const QString &org)
{
    if (org == this->organization()) {
        return;
    }
    m_identity->setOrganization(org);
    Q_EMIT organizationChanged();
}

QString Identity::email() const
{
    return m_identity->primaryEmailAddress();
}

void Identity::setEmail(const QString &email)
{
    if (email == this->email()) {
        return;
    }
    m_identity->setPrimaryEmailAddress(email);
    Q_EMIT emailChanged();
}

uint Identity::uoid() const
{
    return m_identity->uoid();
}

void Identity::setTransport(MailTransport::Transport *transport)
{
    if (transport) {
        m_identity->setTransport(QString::number(transport->id()));
    } else {
        m_identity->setTransport({});
    }
}

QString Identity::signature() const
{
    return m_identity->signature().text();
}

void Identity::setSignature(const QString &sig)
{
    if (!sig.isEmpty()) {
        const KIdentityManagementCore::Signature signature(sig);
        m_identity->setSignature(signature);
    } else {
        m_identity->setSignature(KIdentityManagementCore::Signature());
    }
}

void Identity::setPreferredCryptoMessageFormat(const QString &format)
{
    m_identity->setPreferredCryptoMessageFormat(format);
}

void Identity::setXFace(const QString &xface)
{
    m_identity->setXFaceEnabled(!xface.isEmpty());
    m_identity->setXFace(xface);
}

void Identity::setPgpAutoEncrypt(bool autoencrypt)
{
    m_identity->setPgpAutoEncrypt(autoencrypt);
}

void Identity::setPgpAutoSign(bool autosign)
{
    m_identity->setPgpAutoSign(autosign);
}

void Identity::setKey(GpgME::Protocol protocol, const QByteArray &fingerprint)
{
    if (fingerprint.isEmpty()) {
        m_identity->setPGPEncryptionKey(QByteArray());
        m_identity->setPGPSigningKey(QByteArray());
        m_identity->setSMIMEEncryptionKey(QByteArray());
        m_identity->setSMIMESigningKey(QByteArray());
    } else if (protocol == GpgME::OpenPGP) {
        m_identity->setPGPSigningKey(fingerprint);
        m_identity->setPGPEncryptionKey(fingerprint);
    } else if (protocol == GpgME::CMS) {
        m_identity->setSMIMESigningKey(fingerprint);
        m_identity->setSMIMEEncryptionKey(fingerprint);
    }
}
