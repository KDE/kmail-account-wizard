/*
    SPDX-FileCopyrightText: 2010-2020 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef RESTORELDAPSETTINGSJOB_H
#define RESTORELDAPSETTINGSJOB_H

#include <QObject>
#include <KLDAP/LdapServer>
#include <KConfigGroup>
class KConfig;
namespace KLDAP {
class LdapClientSearchConfig;
}
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
    Q_REQUIRED_RESULT int entry() const;
    void setEntry(int entry);

    KLDAP::LdapClientSearchConfig *clientSearchConfig() const;
    void setClientSearchConfig(KLDAP::LdapClientSearchConfig *clientSearchConfig);

Q_SIGNALS:
    void restoreDone();
private:
    void restore();
    void saveLdapSettings();
    void restoreSettingsDone();
    void loadNextSelectHostSettings();
    void loadNextHostSettings();
    QVector<KLDAP::LdapServer> mSelHosts;
    QVector<KLDAP::LdapServer> mHosts;
    int mEntry = -1;
    int mNumSelHosts = -1;
    int mNumHosts = -1;
    int mCurrentIndex = 0;
    KConfig *mConfig = nullptr;
    KConfigGroup mCurrentGroup;
    KLDAP::LdapClientSearchConfig *mClientSearchConfig = nullptr;
};

#endif // RESTORELDAPSETTINGSJOB_H
