// SPDX-FileCopyrightText: 2010 Omat Holding B.V. <info@omat.nl>
// SPDX-FileCopyrightText: 2014 Sandro Knauß <knauss@kolabsys.com>
// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

#pragma once

#include <KMime/HeaderParsing>
#include <MailTransport/Transport>
#include <QDomElement>
#include <QList>
#include <QString>
#include <optional>

struct Server {
    enum Type {
        IMAP,
        POP3,
        SMTP,
    };
    Type type;
    QString hostname;
    int port = -1;
    QString username;
    MailTransport::Transport::EnumEncryption socketType = MailTransport::Transport::SSL;
    MailTransport::Transport::EnumAuthenticationType authType = MailTransport::Transport::PLAIN;

    [[nodiscard]] QStringList tags() const;

    [[nodiscard]] static std::optional<Server> fromDomElement(const QDomElement &element, const KMime::Types::AddrSpec &addrSpec);
};
QDebug operator<<(QDebug d, const Server &t);
Q_DECLARE_TYPEINFO(Server, Q_MOVABLE_TYPE);

struct EmailProvider {
    QStringList domains;
    QString displayName;
    QString shortDisplayName;

    QList<Server> imapServers;
    QList<Server> popServers;
    QList<Server> smtpServers;
};
QDebug operator<<(QDebug d, const EmailProvider &t);
Q_DECLARE_TYPEINFO(EmailProvider, Q_MOVABLE_TYPE);
