/*
    SPDX-FileCopyrightText: 2010-2020 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "ldap.h"
#include <KLDAP/LdapClientSearchConfig>
#include <KLDAP/AddHostDialog>
#include <KLDAP/LdapClientSearchConfigWriteConfigJob>

#include <KConfig>
#include <KConfigGroup>
#include <KLocalizedString>

Ldap::Ldap(QObject *parent)
    : SetupObject(parent)
    , m_clientSearchConfig(new KLDAP::LdapClientSearchConfig)
    , m_port(389)
    , m_security(KLDAP::LdapServer::None)
    , m_version(3)
    , m_pageSize(0)
    , m_timeLimit(0)
    , m_sizeLimit(0)
    , m_entry(-1)
    , m_editMode(false)
{
}

Ldap::~Ldap()
{
    delete m_clientSearchConfig;
}

KConfig *Ldap::config() const
{
    return m_clientSearchConfig->config();
}

void Ldap::create()
{
    //TODO: use ldapclientsearchconfig to write config
    Q_EMIT info(i18n("Setting up LDAP server..."));

    if (m_server.isEmpty()) {
        Q_EMIT error(i18n("Needed parameters are missing for LDAP config: server '%1'", m_server));
        if (m_editMode) {
            edit();
        }
        return;
    }

    QString host = m_server;

    // Figure out the basedn
    QString basedn = m_baseDn.isEmpty() ? host : m_baseDn;
    if (m_baseDn.isEmpty() && !m_user.isEmpty()) {
        // If the user gave a full email address, the domain name
        // of that overrides the server name for the ldap dn
        const QString user = m_user;
        int pos = user.indexOf(QLatin1Char('@'));
        if (pos > 0) {
            const QString h = user.mid(pos + 1);
            if (!h.isEmpty()) {
                // The user did type in a domain on the email address. Use that
                basedn = h;
                host = h;
            }
        }
    }

    basedn.replace(QLatin1Char('.'), QStringLiteral(",dc="));

    if (!basedn.startsWith(QLatin1String("dc="))) {
        basedn.prepend(QLatin1String("dc="));
    }

    // Set the changes
    KConfig *c = config();
    KConfigGroup group = c->group(QStringLiteral("LDAP"));
    bool hasMyServer = false;
    uint selHosts = group.readEntry("NumSelectedHosts", 0);
    for (uint i = 0; i < selHosts && !hasMyServer; ++i) {
        if (group.readEntry(QStringLiteral("SelectedHost%1").arg(i), QString()) == host) {
            hasMyServer = true;
            m_entry = i;
        }
    }

    if (!hasMyServer) {
        m_entry = selHosts;
        group.writeEntry(QStringLiteral("NumSelectedHosts"), selHosts + 1);
        group.writeEntry(QStringLiteral("SelectedHost%1").arg(selHosts), host);
        group.writeEntry(QStringLiteral("SelectedBase%1").arg(selHosts), basedn);
        group.writeEntry(QStringLiteral("SelectedPort%1").arg(selHosts), m_port);
        group.writeEntry(QStringLiteral("SelectedVersion%1").arg(selHosts), m_version);
        group.writeEntry(QStringLiteral("SelectedSecurity%1").arg(selHosts), securityString());

        if (m_pageSize > 0) {
            group.writeEntry(QStringLiteral("SelectedPageSize%1").arg(selHosts), m_pageSize);
        }

        if (m_timeLimit > 0) {
            group.writeEntry(QStringLiteral("SelectedTimeLimit%1").arg(selHosts), m_timeLimit);
        }

        if (m_sizeLimit > 0) {
            group.writeEntry(QStringLiteral("SelectedSizeLimit%1").arg(selHosts), m_sizeLimit);
        }

        if (!m_authMethod.isEmpty()) {
            group.writeEntry(QStringLiteral("SelectedAuth%1").arg(selHosts), m_authMethod);
            group.writeEntry(QStringLiteral("SelectedBind%1").arg(selHosts), m_bindDn);
            group.writeEntry(QStringLiteral("SelectedPwdBind%1").arg(selHosts), m_password);
            group.writeEntry(QStringLiteral("SelectedRealm%1").arg(selHosts), m_realm);
            group.writeEntry(QStringLiteral("SelectedUser%1").arg(selHosts), m_user);
            group.writeEntry(QStringLiteral("SelectedMech%1").arg(selHosts), m_mech);
        }
        c->sync();
    }
    if (m_editMode) {
        edit();
    }
    Q_EMIT finished(i18n("LDAP set up."));
}

QString Ldap::securityString()
{
    switch (m_security) {
    case KLDAP::LdapServer::None:
        return QStringLiteral("None");
    case KLDAP::LdapServer::SSL:
        return QStringLiteral("SSL");
    case KLDAP::LdapServer::TLS:
        return QStringLiteral("TLS");
    }
}

void Ldap::destroy()
{
    Q_EMIT info(i18n("LDAP not configuring."));
    if (m_entry >= 0) {
        KConfig *c = config();
        KConfigGroup group = c->group(QStringLiteral("LDAP"));
        int cSelHosts = group.readEntry(QStringLiteral("NumSelectedHosts"), 0);
        int cHosts = group.readEntry(QStringLiteral("NumHosts"), 0);
        QVector<KLDAP::LdapServer> selHosts;
        QVector<KLDAP::LdapServer> hosts;
        for (int i = 0; i < cSelHosts; ++i) {
            if (i != m_entry) {
                KLDAP::LdapServer server;
                m_clientSearchConfig->readConfig(server, group, i, true);
                selHosts.append(server);
            }
        }
        hosts.reserve(cHosts);
        for (int i = 0; i < cHosts; ++i) {
            KLDAP::LdapServer server;
            m_clientSearchConfig->readConfig(server, group, i, false);
            hosts.append(server);
        }

        c->deleteGroup(QStringLiteral("LDAP"));
        group = KConfigGroup(c, QStringLiteral("LDAP"));

        for (int i = 0; i < cSelHosts - 1; ++i) {
            auto job = new KLDAP::LdapClientSearchConfigWriteConfigJob;
            job->setActive(true);
            job->setConfig(group);
            job->setServer(selHosts.at(i));
            job->setServerIndex(i);
            job->start();
        }

        for (int i = 0; i < cHosts; ++i) {
            auto job = new KLDAP::LdapClientSearchConfigWriteConfigJob;
            job->setActive(false);
            job->setConfig(group);
            job->setServer(hosts.at(i));
            job->setServerIndex(i);
            job->start();
        }

        group.writeEntry(QStringLiteral("NumSelectedHosts"), cSelHosts - 1);
        group.writeEntry(QStringLiteral("NumHosts"), cHosts);
        c->sync();

        Q_EMIT info(i18n("Removed LDAP entry."));
    }
}

void Ldap::edit()
{
    if (m_entry < 0) {
        Q_EMIT error(i18n("No config found to edit"));
        return;
    }

    KLDAP::LdapServer server;
    KLDAP::LdapClientSearchConfig clientSearchConfig;
    KConfigGroup group = clientSearchConfig.config()->group(QStringLiteral("LDAP"));
    clientSearchConfig.readConfig(server, group, m_entry, true);
    KLDAP::AddHostDialog dlg(&server, nullptr);

    if (dlg.exec() && !server.host().isEmpty()) { //krazy:exclude=crashy
        auto job = new KLDAP::LdapClientSearchConfigWriteConfigJob;
        job->setActive(true);
        job->setConfig(group);
        job->setServer(server);
        job->setServerIndex(m_entry);
        job->start();
    }
}

void Ldap::setUser(const QString &user)
{
    m_user = user;
}

void Ldap::setServer(const QString &server)
{
    m_server = server;
}

void Ldap::setBaseDn(const QString &baseDn)
{
    m_baseDn = baseDn;
}

void Ldap::setAuthenticationMethod(const QString &meth)
{
    m_authMethod = meth;
}

void Ldap::setBindDn(const QString &bindDn)
{
    m_bindDn = bindDn;
}

void Ldap::setPassword(const QString &password)
{
    m_password = password;
}

void Ldap::setPageSize(const int pageSize)
{
    m_pageSize = pageSize;
}

void Ldap::setPort(const int port)
{
    m_port = port;
}

void Ldap::setRealm(const QString &realm)
{
    m_realm = realm;
}

void Ldap::setSaslMech(const QString &saslmech)
{
    m_mech = saslmech;
}

void Ldap::setSecurity(const KLDAP::LdapServer::Security security)
{
    m_security = security;
}

void Ldap::setSizeLimit(const int sizeLimit)
{
    m_sizeLimit = sizeLimit;
}

void Ldap::setTimeLimit(const int timeLimit)
{
    m_timeLimit = timeLimit;
}

void Ldap::setVersion(const int version)
{
    m_version = version;
}

void Ldap::setEditMode(const bool editMode)
{
    m_editMode = editMode;
}
