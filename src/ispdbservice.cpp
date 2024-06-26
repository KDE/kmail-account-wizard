// SPDX-FileCopyrightText: 2010 Omat Holding B.V. <info@omat.nl>
// SPDX-FileCopyrightText: 2014 Sandro Knauß <knauss@kolabsys.com>
// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-FileCopyrightText: 2023-2024 Laurent Montel <montel.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "ispdbservice.h"

#include "accountwizard_debug.h"

#include <KLocalizedString>
#include <QDomDocument>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <qdom.h>

IspdbService::IspdbService(QObject *parent)
    : QObject(parent)
    , mNetworkAccessManager(new QNetworkAccessManager(this))
{
}

void IspdbService::start(const KMime::Types::AddrSpec &addrSpec)
{
    requestConfig(addrSpec, DataBase);
}

void IspdbService::requestConfig(const KMime::Types::AddrSpec &addrSpec, const SearchServerType searchServerType)
{
    const QString domain = addrSpec.domain;
    QUrl url;
    const QString path = QStringLiteral("/mail/config-v1.1.xml");
    switch (searchServerType) {
    case IspHttpsAutoConfig:
        url = QUrl(QStringLiteral("https://autoconfig.") + domain.toLower() + path);
        break;
    case IspAutoConfig:
        url = QUrl(QStringLiteral("http://autoconfig.") + domain.toLower() + path);
        break;
    case IspWellKnow:
        url = QUrl(QStringLiteral("https://") + domain.toLower() + QStringLiteral("/.well-known/autoconfig") + path);
        break;
    case DataBase:
        url = QUrl(QStringLiteral("https://autoconfig.thunderbird.net/v1.1/") + domain.toLower());
    }

    qCDebug(ACCOUNTWIZARD_LOG) << " url " << url;
    QNetworkRequest request(url);
    Q_EMIT requestedConfigFromUrl(url);
    auto reply = mNetworkAccessManager->get(request);

    connect(reply, &QNetworkReply::finished, this, [this, addrSpec, reply, searchServerType]() {
        if (reply->attribute(QNetworkRequest::HttpStatusCodeAttribute) != 200) {
            qCDebug(ACCOUNTWIZARD_LOG) << "Fetching failed" << searchServerType << reply->errorString();
            if (searchServerType != SearchServerType::Last) {
                int index = static_cast<int>(searchServerType);
                requestConfig(addrSpec, static_cast<SearchServerType>(++index));
            }
        }

        handleReply(reply, addrSpec, searchServerType);
        reply->deleteLater();
    });
}

void IspdbService::handleReply(QNetworkReply *const reply, const KMime::Types::AddrSpec &addrSpec, const SearchServerType searchServerType)
{
    const auto data = reply->readAll();
    QDomDocument document;
    const QDomDocument::ParseResult result = document.setContent(data);
    if (!result) {
        qCDebug(ACCOUNTWIZARD_LOG) << "Could not parse xml" << data;
        if (searchServerType == IspWellKnow) { // Last one
            Q_EMIT notConfigFound();
        }
        return;
    }

    const QDomElement docElem = document.documentElement();
    const QDomNodeList emailProviders = docElem.elementsByTagName(QStringLiteral("emailProvider"));

    if (emailProviders.isEmpty()) {
        return;
    }

    EmailProvider emailProvider;

    QDomNode emailProviderNode = emailProviders.at(0).firstChild();
    while (!emailProviderNode.isNull()) {
        const QDomElement element = emailProviderNode.toElement();
        if (element.isNull()) {
            emailProviderNode = emailProviderNode.nextSibling();
            continue;
        }

        const QString tagName(element.tagName());
        if (tagName == QLatin1StringView("domain")) {
            emailProvider.domains << element.text();
        } else if (tagName == QLatin1StringView("displayName")) {
            emailProvider.displayName = element.text();
        } else if (tagName == QLatin1StringView("displayShortName")) {
            emailProvider.shortDisplayName = element.text();
        } else if (tagName == QLatin1StringView("incomingServer") && element.attribute(QStringLiteral("type")) == QLatin1StringView("imap")) {
            auto server = Server::fromDomElement(element, addrSpec);
            if (server) {
                server->type = Server::Type::IMAP;
                emailProvider.imapServers.append(*server);
            }
        } else if (tagName == QLatin1StringView("incomingServer") && element.attribute(QStringLiteral("type")) == QLatin1StringView("pop3")) {
            auto server = Server::fromDomElement(element, addrSpec);
            if (server) {
                server->type = Server::Type::POP3;
                emailProvider.popServers.append(*server);
            }
        } else if (tagName == QLatin1StringView("outgoingServer") && element.attribute(QStringLiteral("type")) == QLatin1StringView("smtp")) {
            auto server = Server::fromDomElement(element, addrSpec);
            if (server) {
                server->type = Server::Type::SMTP;
                emailProvider.smtpServers.append(*server);
            }
        }

        emailProviderNode = emailProviderNode.nextSibling();
    }
    QString foundInServerTypeMessage;
    switch (searchServerType) {
    case IspAutoConfig:
        foundInServerTypeMessage = i18n("Configuration found auto config file.");
        break;
    case IspWellKnow:
        foundInServerTypeMessage = i18n("Configuration found in well-known file.");
        break;
    case DataBase:
        foundInServerTypeMessage = i18n("Configuration found in Mozilla FAI Database.");
        break;
    }

    if (addrSpec.domain.endsWith(QStringLiteral("gmail.com"))) {
        emailProvider.groupware = GMailGroupware{};
    }

    Q_EMIT finished(emailProvider, foundInServerTypeMessage);
}

#include "moc_ispdbservice.cpp"
