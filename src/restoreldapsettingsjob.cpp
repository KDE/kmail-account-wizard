/*
    SPDX-FileCopyrightText: 2010-2020 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "restoreldapsettingsjob.h"
#include <KLDAP/LdapClientSearchConfigWriteConfigJob>
#include <KLDAP/LdapClientSearchConfig>
#include <KConfigGroup>
#include <KConfig>
#include <QDebug>

RestoreLdapSettingsJob::RestoreLdapSettingsJob(QObject *parent)
    : QObject(parent)
{

}

RestoreLdapSettingsJob::~RestoreLdapSettingsJob()
{

}

void RestoreLdapSettingsJob::start()
{
    if (!canStart()) {
        deleteLater();
        qWarning() << "Impossible to start RestoreLdapSettingsJob";
        Q_EMIT restoreDone();
        return;
    }
    restore();
}

void RestoreLdapSettingsJob::restore()
{
    if (mEntry >= 0) {
        KConfigGroup group = mConfig->group(QStringLiteral("LDAP"));
        const int cSelHosts = group.readEntry(QStringLiteral("NumSelectedHosts"), 0);
        const int cHosts = group.readEntry(QStringLiteral("NumHosts"), 0);
        QVector<KLDAP::LdapServer> selHosts;
        QVector<KLDAP::LdapServer> hosts;
        for (int i = 0; i < cSelHosts; ++i) {
            if (i != mEntry) {
                KLDAP::LdapServer server;
                mClientSearchConfig->readConfig(server, group, i, true);
                selHosts.append(server);
            }
        }
        hosts.reserve(cHosts);
        for (int i = 0; i < cHosts; ++i) {
            KLDAP::LdapServer server;
            mClientSearchConfig->readConfig(server, group, i, false);
            hosts.append(server);
        }

        mConfig->deleteGroup(QStringLiteral("LDAP"));
        group = KConfigGroup(mConfig, QStringLiteral("LDAP"));

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
        mConfig->sync();
        Q_EMIT restoreDone();
    }
    //TODO
    deleteLater();
}

KLDAP::LdapClientSearchConfig *RestoreLdapSettingsJob::clientSearchConfig() const
{
    return mClientSearchConfig;
}

void RestoreLdapSettingsJob::setClientSearchConfig(KLDAP::LdapClientSearchConfig *clientSearchConfig)
{
    mClientSearchConfig = clientSearchConfig;
}

int RestoreLdapSettingsJob::entry() const
{
    return mEntry;
}

void RestoreLdapSettingsJob::setEntry(int entry)
{
    mEntry = entry;
}


KConfig *RestoreLdapSettingsJob::config() const
{
    return mConfig;
}

void RestoreLdapSettingsJob::setConfig(KConfig *config)
{
    mConfig = config;
}

bool RestoreLdapSettingsJob::canStart() const
{
    return (mConfig != nullptr) && (mEntry != -1) && mClientSearchConfig;
}
