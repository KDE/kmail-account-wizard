/*
    SPDX-FileCopyrightText: 2010-2020 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "restoreldapsettingsjob.h"
#include <KLDAP/LdapClientSearchConfigWriteConfigJob>
#include <KLDAP/LdapClientSearchConfigReadConfigJob>
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

void RestoreLdapSettingsJob::slotConfigSelectedHostLoaded(const KLDAP::LdapServer &server)
{
    mSelHosts.append(server);
    mCurrentIndex++;
    loadNextSelectHostSettings();
}

void RestoreLdapSettingsJob::loadNextSelectHostSettings()
{
    if (mCurrentIndex < mNumSelHosts) {
        if (mCurrentIndex != mEntry) {
            auto job = new KLDAP::LdapClientSearchConfigReadConfigJob(this);
            connect(job, &KLDAP::LdapClientSearchConfigReadConfigJob::configLoaded, this, &RestoreLdapSettingsJob::slotConfigSelectedHostLoaded);
            job->setActive(true);
            job->setConfig(mCurrentGroup);
            job->setServerIndex(mCurrentIndex);
            job->start();
        } else {
            mCurrentIndex++;
            loadNextSelectHostSettings();
        }
    } else {
        //Reset index;
        mCurrentIndex = 0;
        loadNextHostSettings();
    }
}

void RestoreLdapSettingsJob::slotConfigHostLoaded(const KLDAP::LdapServer &server)
{
    mHosts.append(server);
    mCurrentIndex++;
    loadNextHostSettings();
}

void RestoreLdapSettingsJob::loadNextHostSettings()
{
    if (mCurrentIndex < mNumHosts) {
        auto job = new KLDAP::LdapClientSearchConfigReadConfigJob(this);
        connect(job, &KLDAP::LdapClientSearchConfigReadConfigJob::configLoaded, this, &RestoreLdapSettingsJob::slotConfigHostLoaded);
        job->setActive(false);
        job->setConfig(mCurrentGroup);
        job->setServerIndex(mCurrentIndex);
        job->start();
    } else {
        saveLdapSettings();
    }
}

void RestoreLdapSettingsJob::restore()
{
    if (mEntry >= 0) {
        mCurrentGroup = mConfig->group(QStringLiteral("LDAP"));
        mNumSelHosts = mCurrentGroup.readEntry(QStringLiteral("NumSelectedHosts"), 0);
        mNumHosts = mCurrentGroup.readEntry(QStringLiteral("NumHosts"), 0);
        loadNextSelectHostSettings();
    } else {
        restoreSettingsDone();
    }
}

void RestoreLdapSettingsJob::restoreSettingsDone()
{
    Q_EMIT restoreDone();
    deleteLater();
}

void RestoreLdapSettingsJob::saveLdapSettings()
{
    mConfig->deleteGroup(QStringLiteral("LDAP"));
    mCurrentGroup = KConfigGroup(mConfig, QStringLiteral("LDAP"));

    //Move async
    for (int i = 0; i < mNumSelHosts - 1; ++i) {
        auto job = new KLDAP::LdapClientSearchConfigWriteConfigJob;
        job->setActive(true);
        job->setConfig(mCurrentGroup);
        job->setServer(mSelHosts.at(i));
        job->setServerIndex(i);
        job->start();
    }

    for (int i = 0; i < mNumHosts; ++i) {
        auto job = new KLDAP::LdapClientSearchConfigWriteConfigJob;
        job->setActive(false);
        job->setConfig(mCurrentGroup);
        job->setServer(mHosts.at(i));
        job->setServerIndex(i);
        job->start();
    }

    mCurrentGroup.writeEntry(QStringLiteral("NumSelectedHosts"), mNumSelHosts - 1);
    mCurrentGroup.writeEntry(QStringLiteral("NumHosts"), mNumHosts);
    mConfig->sync();
    restoreSettingsDone();
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
