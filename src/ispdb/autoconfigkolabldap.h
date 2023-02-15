/*
 * SPDX-FileCopyrightText: 2014 Sandro Knauß <knauss@kolabsys.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#pragma once

#include "autoconfigkolabmail.h"
#include <kldap/ldapserver.h>

struct ldapServer;

class AutoconfigKolabLdap : public AutoconfigKolabMail
{
    Q_OBJECT
public:
    /** Constructor */
    explicit AutoconfigKolabLdap(QObject *parent = nullptr);

    Q_REQUIRED_RESULT QHash<QString, ldapServer> ldapServers() const;

protected:
    void lookupInDb(bool auth, bool crypt) override;
    void parseResult(const QDomDocument &document) override;

private:
    Q_REQUIRED_RESULT ldapServer createLdapServer(const QDomElement &n);

    QHash<QString, ldapServer> mLdapServers;
};

struct ldapServer {
    ldapServer()
        : port(-1)
        , socketType(KLDAP::LdapServer::None)
        , authentication(KLDAP::LdapServer::Anonymous)
        , ldapVersion(3)
        , pageSize(-1)
        , timeLimit(-1)
        , sizeLimit(-1)
    {
    }

    bool isValid() const;
    QString hostname;
    QString bindDn;
    QString password;
    QString saslMech;
    QString username;
    QString realm;
    QString dn;
    QString filter;
    int port;
    KLDAP::LdapServer::Security socketType;
    KLDAP::LdapServer::Auth authentication;
    int ldapVersion;
    int pageSize;
    int timeLimit;
    int sizeLimit;
};

