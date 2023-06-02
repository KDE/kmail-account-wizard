// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

#pragma once
#include "ispdb/configurationmodel.h"
#include "libaccountwizard_export.h"
#include "manualconfiguration.h"
#include <QObject>

class Identity;
class IspdbService;

class LIBACCOUNTWIZARD_EXPORT SetupManager : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString email READ email WRITE setEmail NOTIFY emailChanged)
    Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged)
    Q_PROPERTY(QString fullName READ fullName WRITE setFullName NOTIFY fullNameChanged)
    Q_PROPERTY(Identity *identity READ identity CONSTANT)
    Q_PROPERTY(ConfigurationModel *configurationModel READ configurationModel CONSTANT)
    Q_PROPERTY(QString searchIspdbFoundMessage MEMBER m_searchIspdbFoundMessage NOTIFY searchIspdbFoundMessageChanged)
    Q_PROPERTY(bool noConfigFound MEMBER m_noConfigFound NOTIFY noConfigFoundChanged)
    Q_PROPERTY(ManualConfiguration *manualConfiguration READ manualConfiguration CONSTANT)

public:
    explicit SetupManager(QObject *parent = nullptr);
    ~SetupManager();

    Q_REQUIRED_RESULT QString fullName() const;
    void setFullName(const QString &fullName);

    Q_REQUIRED_RESULT QString email() const;
    void setEmail(const QString &email);

    Q_REQUIRED_RESULT QString password() const;
    void setPassword(const QString &password);

    Identity *identity() const;
    ConfigurationModel *configurationModel() const;
    ManualConfiguration *manualConfiguration() const;

    Q_INVOKABLE void searchConfiguration();
    Q_INVOKABLE void createAutomaticAccount();
    Q_INVOKABLE void createManualAccount();

Q_SIGNALS:
    void fullNameChanged();
    void emailChanged();
    void passwordChanged();
    void searchIspdbFoundMessageChanged();
    void noConfigFoundChanged();

private:
    void setEmailProvider(const EmailProvider &emailProvider, const QString &messageInfo);
    void setErrorOccured(const QString &errorMessage);
    void clearConfiguration();
    void noConfigFound();
    QString m_password;
    QString m_searchIspdbFoundMessage;
    Identity *const m_identity;
    IspdbService *const m_ispdbService;
    ConfigurationModel *const m_configurationModel;
    ManualConfiguration *const m_manualConfiguration;
    bool m_noConfigFound = false;
};
