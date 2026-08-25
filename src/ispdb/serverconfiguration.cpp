// SPDX-FileCopyrightText: 2010 Omat Holding B.V. <info@omat.nl>
// SPDX-FileCopyrightText: 2014 Sandro Knauß <knauss@kolabsys.com>
// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "ispdb/serverconfiguration.h"
#include <KAccountAutoconfig/KAutoconfigServer>
#include <KLocalizedString>

static QString replacePlaceholders(const QString &in, const KMime::Types::AddrSpec &addrSpec)
{
    QString out(in);
    out.replace(QLatin1StringView("%EMAILLOCALPART%"), addrSpec.localPart);
    out.replace(QLatin1StringView("%EMAILADDRESS%"), addrSpec.asString());
    out.replace(QLatin1StringView("%EMAILDOMAIN%"), addrSpec.domain);
    return out;
}

QStringList Server::tags() const
{
    QStringList tags;
    switch (type) {
    case IMAP:
        tags << i18n("IMAP");
        break;
    case POP3:
        tags << i18n("POP3");
        break;
    case SMTP:
        tags << i18n("SMTP");
        break;
    }

    switch (socketType) {
    case MailTransport::TransportBase::SSL:
        tags << i18n("SSL/TLS");
        break;
    case MailTransport::TransportBase::TLS:
        tags << i18n("StartTLS");
        break;
    case MailTransport::TransportBase::None:
        tags << i18nc("No security mechanism", "None");
        break;
    }

    return tags;
}

std::optional<Server::Type> fromProtocol(KAutoconfigServer::Protocol protocol)
{
    switch (protocol) {
    case KAutoconfigServer::IMAP:
        return Server::IMAP;
    case KAutoconfigServer::POP3:
        return Server::POP3;
    case KAutoconfigServer::SMTP:
        return Server::SMTP;
    default:
        return {};
    }
}

std::optional<MailTransport::Transport::EnumAuthenticationType> fromAuthenticationType(KAutoconfigServer::AuthenticationType authType)
{
    switch (authType) {
    case KAutoconfigServer::PasswordCleartext:
        return MailTransport::TransportBase::PLAIN;
    case KAutoconfigServer::PasswordEncrypted:
        return MailTransport::TransportBase::CRAM_MD5;
    case KAutoconfigServer::Digest:
    case KAutoconfigServer::Basic:
    case KAutoconfigServer::UnknownAuthentication:
    default:
        return {};
    }
}

std::optional<MailTransport::Transport::EnumEncryption> fromSocketType(KAutoconfigServer::SocketType socketType)
{
    switch (socketType) {
    case KAutoconfigServer::Plain:
        return MailTransport::TransportBase::None;
    case KAutoconfigServer::SSL:
        return MailTransport::TransportBase::SSL;
    case KAutoconfigServer::STARTTLS:
        return MailTransport::TransportBase::TLS;
    case KAutoconfigServer::UnknownSocketType:
    default:
        return {};
    }
}

std::optional<Server> Server::fromKAutoconfigServer(const KAutoconfigServer &autoconfigServer, const KMime::Types::AddrSpec &addrSpec)
{
    Server server;
    if (const auto type = fromProtocol(autoconfigServer.protocol())) {
        server.type = type.value();
    } else {
        return {};
    }
    server.hostname = autoconfigServer.url().host();
    server.port = autoconfigServer.url().port();
    server.username = addrSpec.asString();
    if (const auto authType = fromAuthenticationType(autoconfigServer.authenticationType())) {
        server.authType = authType.value();
    }
    if (const auto socketType = fromSocketType(autoconfigServer.socketType())) {
        server.socketType = socketType.value();
    }
    return server;
}

QDebug operator<<(QDebug d, const EmailProvider &t)
{
    d.space() << "domains" << t.domains;
    d.space() << "displayName" << t.displayName;
    d.space() << "shortDisplayName" << t.shortDisplayName;
    d.space() << "imapServers" << t.imapServers;
    d.space() << "popServers" << t.popServers;
    d.space() << "smtpServers" << t.smtpServers;

    return d;
}

QDebug operator<<(QDebug d, const Server &t)
{
    d.space() << "type" << t.type;
    d.space() << "hostname" << t.hostname;
    d.space() << "port" << t.port;
    d.space() << "username" << t.username;
    d.space() << "socketType" << t.socketType;
    d.space() << "authType" << t.authType;
    return d;
}
