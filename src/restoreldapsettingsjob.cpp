/*
    SPDX-FileCopyrightText: 2010-2020 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "restoreldapsettingsjob.h"
#include <KLDAP/LdapClientSearchConfigWriteConfigJob>
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
    //TODO
    deleteLater();
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
    return (mConfig != nullptr);
}
