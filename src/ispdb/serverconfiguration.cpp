// SPDX-FileCopyrightText: 2010 Omat Holding B.V. <info@omat.nl>
// SPDX-FileCopyrightText: 2014 Sandro Knauß <knauss@kolabsys.com>
// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "ispdb/serverconfiguration.h"
#include <KLocalizedString>

QString replacePlaceholders(const QString &in, const KMime::Types::AddrSpec &addrSpec)
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

std::optional<Server> Server::fromDomElement(const QDomElement &element, const KMime::Types::AddrSpec &addrSpec)
{
    QDomNode o = element.firstChild();
    Server server;
    while (!o.isNull()) {
        QDomElement f = o.toElement();
        if (f.isNull()) {
            o = o.nextSibling();
            continue;
        }

        const QString tagName(f.tagName());
        if (tagName == QLatin1StringView("hostname")) {
            server.hostname = replacePlaceholders(f.text(), addrSpec);
        } else if (tagName == QLatin1StringView("port")) {
            server.port = f.text().toInt();
        } else if (tagName == QLatin1StringView("socketType")) {
            const QString type(f.text());
            if (type == QLatin1StringView("plain")) {
                server.socketType = MailTransport::TransportBase::None;
            } else if (type == QLatin1StringView("SSL")) {
                server.socketType = MailTransport::TransportBase::SSL;
            }
            if (type == QLatin1StringView("STARTTLS")) {
                server.socketType = MailTransport::TransportBase::TLS;
            }
        } else if (tagName == QLatin1StringView("username")) {
            server.username = replacePlaceholders(f.text(), addrSpec);
        } else if (tagName == QLatin1StringView("authentication") && server.authType == MailTransport::Transport::PLAIN) {
            const QString type(f.text());
            if (type == QLatin1StringView("password-cleartext") || type == QLatin1StringView("plain")) {
                server.authType = MailTransport::TransportBase::PLAIN;
            } else if (type == QLatin1StringView("password-encrypted") || type == QLatin1StringView("secure")) {
                server.authType = MailTransport::TransportBase::CRAM_MD5;
            } else if (type == QLatin1StringView("NTLM")) {
                server.authType = MailTransport::TransportBase::NTLM;
            } else if (type == QLatin1StringView("GSSAPI")) {
                server.authType = MailTransport::TransportBase::GSSAPI;
            } else if (type == QLatin1StringView("client-ip-based")) {
                server.authType = MailTransport::TransportBase::ANONYMOUS;
            } else if (type == QLatin1StringView("none")) {
                server.authType = MailTransport::TransportBase::ANONYMOUS;
            } else if (type == QLatin1StringView("OAuth2")) {
                server.authType = MailTransport::TransportBase::XOAUTH2;
            }
        }
        o = o.nextSibling();
    }
    if (server.port == -1) {
        return std::nullopt;
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
