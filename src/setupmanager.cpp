// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "setupmanager.h"

#include "identity.h"
#include "ispdbservice.h"
#include <KEMailSettings>

SetupManager::SetupManager(QObject *parent)
    : QObject(parent)
    , m_identity(new Identity(this))
    , m_ispdbService(new IspdbService(this))
    , m_configurationModel(new ConfigurationModel(this))
{
    KEMailSettings emailSettings;
    setFullName(emailSettings.getSetting(KEMailSettings::RealName));
    setEmail(emailSettings.getSetting(KEMailSettings::EmailAddress));

    connect(m_identity, &Identity::emailChanged, this, &SetupManager::emailChanged);
    connect(m_ispdbService, &IspdbService::finished, this, &SetupManager::setEmailProvider);
    connect(m_ispdbService, &IspdbService::errorOccured, this, &SetupManager::setErrorOccured);
#if 0
    connect(m_ispdbService, &IspdbService::info, m_configurationModel,
            &ConfigurationModel::setInformation);
#endif
}

SetupManager::~SetupManager() = default;

QString SetupManager::fullName() const
{
    return m_identity->fullName();
}

void SetupManager::setFullName(const QString &fullName)
{
    m_identity->setFullName(fullName);
}

QString SetupManager::email() const
{
    return m_identity->email();
}

void SetupManager::setEmail(const QString &email)
{
    m_identity->setEmail(email);
}

QString SetupManager::password() const
{
    return m_password;
}

void SetupManager::setPassword(const QString &password)
{
    if (m_password == password) {
        return;
    }
    m_password = password;
    Q_EMIT passwordChanged();
}

Identity *SetupManager::identity() const
{
    return m_identity;
}

ConfigurationModel *SetupManager::configurationModel() const
{
    return m_configurationModel;
}

void SetupManager::searchConfiguration()
{
    clearConfiguration();
    KMime::Types::Mailbox box;
    box.fromUnicodeString(email());
    m_ispdbService->start(box.addrSpec());
}

void SetupManager::setEmailProvider(const EmailProvider &emailProvider, const QString &messageInfo)
{
    m_searchIspdbFoundMessage = messageInfo;
    m_configurationModel->setEmailProvider(emailProvider);
    Q_EMIT searchIspdbFoundMessageChanged();
}

void SetupManager::clearConfiguration()
{
    m_configurationModel->clear();
    m_searchIspdbFoundMessage.clear();
    Q_EMIT searchIspdbFoundMessageChanged();

    m_searchIspdbErrorMessage.clear();
    Q_EMIT searchIspdbErrorMessageChanged();
}

void SetupManager::setErrorOccured(const QString &errorMessage)
{
    m_searchIspdbErrorMessage = errorMessage;
    Q_EMIT searchIspdbErrorMessageChanged();
}
