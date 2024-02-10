// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-FileCopyrightText: 2023-2024 Laurent Montel <montel.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "setupmanager.h"

#include "accountwizard_debug.h"
#include "ispdbservice.h"
#include <KEMailSettings>

SetupManager::SetupManager(QObject *parent)
    : QObject(parent)
    , mIdentity(new IdentityImpl(this))
    , mIspdbService(new IspdbService(this))
    , mConfigurationModel(new ConfigurationModel(this))
    , mManualConfiguration(new ManualConfigurationImpl(this))
{
    KEMailSettings emailSettings;
    setFullName(emailSettings.getSetting(KEMailSettings::RealName));
    setEmail(emailSettings.getSetting(KEMailSettings::EmailAddress));

    connect(mIdentity, &IdentityBase::emailChanged, this, &SetupManager::emailChanged);
    connect(mIspdbService, &IspdbService::finished, this, &SetupManager::setEmailProvider);
    connect(mIspdbService, &IspdbService::notConfigFound, this, &SetupManager::noConfigFound);

    connect(mManualConfiguration, &ManualConfigurationBase::error, this, &SetupManager::slotError);
    connect(mManualConfiguration, &ManualConfigurationBase::finished, this, &SetupManager::slotFinished);
    connect(mManualConfiguration, &ManualConfigurationBase::info, this, &SetupManager::slotInfo);
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

IdentityBase *SetupManager::identity() const
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

void SetupManager::createAutomaticAccount(int index)
{
    qCDebug(ACCOUNTWIZARD_LOG) << " Create Automatic Account" << index;
    const auto configuration = configurationModel()->configuration(index);
    qCDebug(ACCOUNTWIZARD_LOG) << configuration.incoming;

    mManualConfiguration->setIncomingHostName(configuration.incoming.hostname);
    mManualConfiguration->setIncomingPort(configuration.incoming.port);
    mManualConfiguration->setIncomingUserName(configuration.incoming.username);
    switch (configuration.incoming.type) {
    case Server::Type::POP3:
        mManualConfiguration->setCurrentIncomingProtocol(0);
        break;
    case Server::Type::IMAP:
        mManualConfiguration->setCurrentIncomingProtocol(1);
        break;
    case Server::Type::SMTP:
        Q_UNREACHABLE();
    }
    switch (configuration.incoming.authType) {
        // keep in sync with ordering in ManualConfigurationBase::authenticationProtocols
        using KIMAPAuth = KIMAP::LoginJob::AuthenticationMode;
    case AuthType::Plain:
        mManualConfiguration->setCurrentIncomingAuthenticationProtocol(KIMAPAuth::Plain);
        break;
    case AuthType::CramMD5:
        mManualConfiguration->setCurrentIncomingAuthenticationProtocol(KIMAPAuth::CramMD5);
        break;
    case AuthType::NTLM:
        mManualConfiguration->setCurrentIncomingAuthenticationProtocol(KIMAPAuth::NTLM);
        break;
    case AuthType::GSSAPI:
        mManualConfiguration->setCurrentIncomingAuthenticationProtocol(KIMAPAuth::GSSAPI);
        break;
    case AuthType::ClientIP:
    case AuthType::NoAuth:
        mManualConfiguration->setCurrentIncomingAuthenticationProtocol(KIMAPAuth::Anonymous);
        break;
    case AuthType::OAuth2:
        mManualConfiguration->setCurrentIncomingAuthenticationProtocol(KIMAPAuth::XOAuth2);
        break;
    }
    mManualConfiguration->setCurrentIncomingProtocol(configuration.incoming.type);
    mManualConfiguration->setPassword(mPassword);

    if (configuration.outgoing) {
        mManualConfiguration->setOutgoingHostName(configuration.outgoing->hostname);
        mManualConfiguration->setOutgoingPort(configuration.outgoing->port);
        mManualConfiguration->setOutgoingUserName(configuration.outgoing->username);
        mManualConfiguration->setPassword(mPassword);
    }

    return;

    mIdentity->create();
    const uint id = mIdentity->uoid();
    mManualConfiguration->setIdentityId(id);
    mManualConfiguration->createManualAccount();

    mAccountCreated = true;
}

void SetupManager::createManualAccount()
{
    qCDebug(ACCOUNTWIZARD_LOG) << " Create Manual Account";
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
    mAccountCreated = false;
    Q_EMIT noConfigFoundChanged();
}

void SetupManager::noConfigFound()
{
    mNoConfigFound = true;
    Q_EMIT noConfigFoundChanged();
}

ManualConfigurationBase *SetupManager::manualConfiguration() const
{
    return mManualConfiguration;
}

#include "moc_setupmanager.cpp"
