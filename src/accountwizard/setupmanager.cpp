// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-FileCopyrightText: 2023-2024 Laurent Montel <montel.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "setupmanager.h"

#include "accountwizard_debug.h"
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

    connect(mManualConfiguration, &ManualConfiguration::error, this, &SetupManager::slotError);
    connect(mManualConfiguration, &ManualConfiguration::finished, this, &SetupManager::slotFinished);
    connect(mManualConfiguration, &ManualConfiguration::info, this, &SetupManager::slotInfo);
}

SetupManager::~SetupManager()
{
    if (!mAccountCreated) {
        mIdentity->destroy();
    }
}

void SetupManager::slotError(const QString &str)
{
    mDetails += str + QLatin1Char('\n');
    // TODO add color ?
    Q_EMIT detailsChanged();
}

void SetupManager::slotFinished(const QString &str)
{
    mDetails += str + QLatin1Char('\n');
    // TODO add color ?
    Q_EMIT detailsChanged();
}

void SetupManager::slotInfo(const QString &str)
{
    mDetails += str + QLatin1Char('\n');
    // TODO add color ?
    Q_EMIT detailsChanged();
}

QString SetupManager::details() const
{
    return mDetails;
}

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
    mManualConfiguration->setEmail(email);
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
    qCDebug(ACCOUNTWIZARD_LOG) << " Create Automatic Account";
    mAccountCreated = true;
}

void SetupManager::createManualAccount()
{
    qCDebug(ACCOUNTWIZARD_LOG) << " Create MAnual Account";
    mIdentity->create();
    const uint id = mIdentity->uoid();
    mManualConfiguration->setIdentityId(id);
    mManualConfiguration->createManualAccount();
    mAccountCreated = true;
}

void SetupManager::setEmailProvider(const EmailProvider &emailProvider, const QString &messageInfo)
{
    qCDebug(ACCOUNTWIZARD_LOG) << " emailProvider " << emailProvider;
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
