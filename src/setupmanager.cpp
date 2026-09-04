// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-FileCopyrightText: 2023-2026 Laurent Montel <montel@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "setupmanager.h"

#include "accountwizard_debug.h"
#include "ispdbservice.h"
#include <KEMailSettings>
#include <QUrl>

SetupManager::SetupManager(QObject *parent)
    : QObject(parent)
    , mIspdbService(new IspdbService(this))
    , mConfigurationModel(new ConfigurationModel(this))
{
    KEMailSettings emailSettings;
    setFullName(emailSettings.getSetting(KEMailSettings::RealName));
    setEmail(emailSettings.getSetting(KEMailSettings::EmailAddress));

    connect(mIspdbService, &IspdbService::finished, this, &SetupManager::setEmailProvider);
    connect(mIspdbService, &IspdbService::notConfigFound, this, &SetupManager::noConfigFound);
}

SetupManager::~SetupManager()
{
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
    return mFullName;
}

void SetupManager::setFullName(const QString &fullName)
{
    mFullName = fullName;
    mConfigurationModel->setFullName(fullName);
}

QString SetupManager::email() const
{
    return mEmail;
}

void SetupManager::setEmail(const QString &email)
{
    mEmail = email;
    mConfigurationModel->setEmail(email);
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
    mConfigurationModel->setPassword(password);
    Q_EMIT passwordChanged();
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
QString SetupManager::toAce(const QString &idnString)
{
    return QString::fromLatin1(QUrl::toAce(idnString));
}

void SetupManager::setEmailProvider(const EmailProvider &emailProvider)
{
    qCDebug(ACCOUNTWIZARD_LOG) << " emailProvider " << emailProvider;
    mConfigurationModel->setEmailProvider(emailProvider);
}

void SetupManager::clearConfiguration()
{
    mConfigurationModel->clear();

    mNoConfigFound = false;
    Q_EMIT noConfigFoundChanged();
}

void SetupManager::noConfigFound()
{
    mNoConfigFound = true;
    Q_EMIT noConfigFoundChanged();
}

#include "moc_setupmanager.cpp"
