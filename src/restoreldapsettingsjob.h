/*
    SPDX-FileCopyrightText: 2010-2020 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef RESTORELDAPSETTINGSJOB_H
#define RESTORELDAPSETTINGSJOB_H

#include <QObject>
#include <KLDAP/LdapServer>
class KConfig;
class RestoreLdapSettingsJob : public QObject
{
    Q_OBJECT
public:
    explicit RestoreLdapSettingsJob(QObject *parent = nullptr);
    ~RestoreLdapSettingsJob() override;

    void start();
    KConfig *config() const;
    void setConfig(KConfig *config);
    Q_REQUIRED_RESULT bool canStart() const;
Q_SIGNALS:
    void restoreDone();
private:
    QVector<KLDAP::LdapServer> mSelHosts;
    QVector<KLDAP::LdapServer> mHosts;
    KConfig *mConfig = nullptr;
};

#endif // RESTORELDAPSETTINGSJOB_H
