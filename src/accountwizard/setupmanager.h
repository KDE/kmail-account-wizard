// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

#pragma once
#include "accountconfigurationimpl.h"
#include "identityimpl.h"
#include "ispdb/configurationmodel.h"
#include "libaccountwizard_export.h"
#include <QObject>

class IdentityImpl;
class IspdbService;

class LIBACCOUNTWIZARD_EXPORT SetupManager : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString email READ email WRITE setEmail NOTIFY emailChanged)
    Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged)
    Q_PROPERTY(QString fullName READ fullName WRITE setFullName NOTIFY fullNameChanged)
    Q_PROPERTY(IdentityBase *identity READ identity CONSTANT)
    Q_PROPERTY(ConfigurationModel *configurationModel READ configurationModel CONSTANT)
    Q_PROPERTY(QString searchIspdbFoundMessage MEMBER mSearchIspdbFoundMessage NOTIFY searchIspdbFoundMessageChanged)
    Q_PROPERTY(bool noConfigFound MEMBER mNoConfigFound NOTIFY noConfigFoundChanged)
    Q_PROPERTY(AccountConfigurationBase *manualConfiguration READ manualConfiguration CONSTANT)
    Q_PROPERTY(QString details READ details NOTIFY detailsChanged)

public:
    explicit SetupManager(QObject *parent = nullptr);
    ~SetupManager() override;

    [[nodiscard]] QString fullName() const;
    void setFullName(const QString &fullName);

    [[nodiscard]] QString email() const;
    void setEmail(const QString &email);

    [[nodiscard]] QString password() const;
    void setPassword(const QString &password);

    IdentityBase *identity() const;
    ConfigurationModel *configurationModel() const;
    AccountConfigurationBase *manualConfiguration() const;

    Q_INVOKABLE void searchConfiguration();
    Q_INVOKABLE void createAutomaticAccount();
    Q_INVOKABLE void createManualAccount();

    [[nodiscard]] QString details() const;

Q_SIGNALS:
    void fullNameChanged();
    void emailChanged();
    void passwordChanged();
    void searchIspdbFoundMessageChanged();
    void noConfigFoundChanged();
    void detailsChanged();

private:
    LIBACCOUNTWIZARD_NO_EXPORT void setEmailProvider(const EmailProvider &emailProvider, const QString &messageInfo);
    LIBACCOUNTWIZARD_NO_EXPORT void clearConfiguration();
    LIBACCOUNTWIZARD_NO_EXPORT void noConfigFound();
    LIBACCOUNTWIZARD_NO_EXPORT void slotError(const QString &str);
    LIBACCOUNTWIZARD_NO_EXPORT void slotFinished(const QString &str);
    LIBACCOUNTWIZARD_NO_EXPORT void slotInfo(const QString &str);
    QString mPassword;
    QString mSearchIspdbFoundMessage;
    QString mDetails;
    IdentityImpl *const mIdentity;
    IspdbService *const mIspdbService;
    ConfigurationModel *const mConfigurationModel;
    AccountConfigurationImpl *const mManualConfiguration;
    bool mNoConfigFound = false;
    bool mAccountCreated = false;
};
