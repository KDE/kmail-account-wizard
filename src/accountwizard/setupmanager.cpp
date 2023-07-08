// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-FileCopyrightText: 2023 Laurent Montel <montel@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "setupmanager.h"

#include "identity.h"
#include "ispdbservice.h"
#include <KEMailSettings>

SetupManager::SetupManager(QObject *parent)
    : QObject(parent)
    , mIdentity(new Identity(this))
    , mIspdbService(new IspdbService(this))
    , mConfigurationModel(new ConfigurationModel(this))
    , mManualConfiguration(new ManualConfiguration(this))
{
    KEMailSettings emailSettings;
    setFullName(emailSettings.getSetting(KEMailSettings::RealName));
    setEmail(emailSettings.getSetting(KEMailSettings::EmailAddress));

    connect(mIdentity, &Identity::emailChanged, this, &SetupManager::emailChanged);
    connect(mIspdbService, &IspdbService::finished, this, &SetupManager::setEmailProvider);
    connect(mIspdbService, &IspdbService::notConfigFound, this, &SetupManager::noConfigFound);
}

SetupManager::~SetupManager() = default;

QString SetupManager::fullName() const
{
    return mIdentity->fullName();
}

void SetupManager::setFullName(const QString &fullName)
{
    mIdentity->setFullName(fullName);
}

QString SetupManager::email() const
{
    return mIdentity->email();
}

void SetupManager::setEmail(const QString &email)
{
    mIdentity->setEmail(email);
    clearConfiguration();
}

QString SetupManager::password() const
{
    return mPassword;
}

void SetupManager::setPassword(const QString &password)
{
    if (mPassword == password) {
        return;
    }
    mPassword = password;
    Q_EMIT passwordChanged();
}

Identity *SetupManager::identity() const
{
    return mIdentity;
}

ConfigurationModel *SetupManager::configurationModel() const
{
    return mConfigurationModel;
}

void SetupManager::searchConfiguration()
{
    clearConfiguration();
    KMime::Types::Mailbox box;
    box.fromUnicodeString(email());
    mIspdbService->start(box.addrSpec());
}

void SetupManager::createAutomaticAccount()
{
    qDebug() << " Create Automatic Account";
}

void SetupManager::createManualAccount()
{
    qDebug() << " Create MAnual Account";
    mManualConfiguration->createManualAccount();
}

void SetupManager::setEmailProvider(const EmailProvider &emailProvider, const QString &messageInfo)
{
    mSearchIspdbFoundMessage = messageInfo;
    mConfigurationModel->setEmailProvider(emailProvider);
    Q_EMIT searchIspdbFoundMessageChanged();
}

void SetupManager::clearConfiguration()
{
    mConfigurationModel->clear();
    mSearchIspdbFoundMessage.clear();
    Q_EMIT searchIspdbFoundMessageChanged();

    mNoConfigFound = false;
    Q_EMIT noConfigFoundChanged();
}

void SetupManager::noConfigFound()
{
    mNoConfigFound = true;
    Q_EMIT noConfigFoundChanged();
}

ManualConfiguration *SetupManager::manualConfiguration() const
{
    return mManualConfiguration;
}

#include "moc_setupmanager.cpp"
