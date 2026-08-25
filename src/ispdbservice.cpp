// SPDX-FileCopyrightText: 2010 Omat Holding B.V. <info@omat.nl>
// SPDX-FileCopyrightText: 2014 Sandro Knauß <knauss@kolabsys.com>
// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-FileCopyrightText: 2023-2026 Laurent Montel <montel@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "ispdbservice.h"

#include "accountwizard_debug.h"

#include <KAccountAutoconfig/KAccountAutoconfigJob>

IspdbService::IspdbService(QObject *parent)
    : QObject(parent)
{
}

void IspdbService::start(const KMime::Types::AddrSpec &addrSpec)
{
    const auto job = new KAccountAutoconfigJob(this);
    job->setEmail(addrSpec.asString());
    job->start();

    connect(job, &KJob::result, this, [this, addrSpec, job]() {
        if (job->error()) {
            qCDebug(ACCOUNTWIZARD_LOG) << "Job failed" << job->errorString();
            Q_EMIT notConfigFound();
        } else {
            handleReply(job->servers(), addrSpec);
        }
        job->deleteLater();
    });
}

void IspdbService::handleReply(const QList<KAutoconfigServer> &servers, const KMime::Types::AddrSpec &addrSpec)
{
    EmailProvider emailProvider;
    for (const auto &server : servers) {
        switch (server.protocol()) {
        case KAutoconfigServer::IMAP:
            if (const auto convertedServ = Server::fromKAutoconfigServer(server, addrSpec)) {
                emailProvider.imapServers.append(convertedServ.value());
            }
            break;
        case KAutoconfigServer::POP3:
            if (const auto convertedServ = Server::fromKAutoconfigServer(server, addrSpec)) {
                emailProvider.popServers.append(convertedServ.value());
            }
            break;
        case KAutoconfigServer::SMTP:
            if (const auto convertedServ = Server::fromKAutoconfigServer(server, addrSpec)) {
                emailProvider.smtpServers.append(convertedServ.value());
            }
            break;
        default:
            break;
        }
    }

    if (addrSpec.domain.endsWith(QStringLiteral("gmail.com"))) {
        emailProvider.groupware = GMailGroupware{};
    }

    Q_EMIT finished(emailProvider);
}

#include "moc_ispdbservice.cpp"
