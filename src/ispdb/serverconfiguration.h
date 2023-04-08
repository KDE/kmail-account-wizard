// SPDX-FileCopyrightText: 2010 Omat Holding B.V. <info@omat.nl>
// SPDX-FileCopyrightText: 2014 Sandro Knauß <knauss@kolabsys.com>
// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

#pragma once

#include <QString>
#include <QDomElement>
#include <QList>
#include <optional>
#include <KMime/HeaderParsing>

enum SocketType {
    SSL = 0, ///< SSL socket, the most secure and the default
    StartTLS,
    None,
};

 /// Ispdb uses custom authtyps, hence the enum here.
 /// @see https://wiki.mozilla.org/Thunderbird:Autoconfiguration:ConfigFileFormat
 /// In particular, note that Ispdb's Plain represents both Cleartext and AUTH Plain
 /// We will always treat it as Cleartext
enum AuthType {
    Plain = 0,
    CramMD5,
    NTLM,
    GSSAPI,
    ClientIP,
    NoAuth,
    Basic,
    OAuth2,
};

struct Server {
    enum Type {
        IMAP,
        POP3,
        SMTP
    };
    Type type;
    QString hostname;
    int port;
    QString username;
    SocketType socketType = SSL;
    AuthType authType = Plain;

    QStringList tags() const;

    static std::optional<Server> fromDomElement(const QDomElement &element, const KMime::Types::AddrSpec &addrSpec);
};

struct EmailProvider {
    QStringList domains;
    QString displayName;
    QString shortDisplayName;

    QVector<Server> imapServers;
    QVector<Server> popServers;
    QVector<Server> smtpServers;
};
