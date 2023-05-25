// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

#pragma once

#include "ispdb/configurationmodel.h"
#include <QObject>

class Identity;
class IspdbService;

class SetupManager : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString email READ email WRITE setEmail NOTIFY emailChanged)
    Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged)
    Q_PROPERTY(QString fullName READ fullName WRITE setFullName NOTIFY fullNameChanged)
    Q_PROPERTY(Identity *identity READ identity CONSTANT)
    Q_PROPERTY(ConfigurationModel *configurationModel READ configurationModel CONSTANT)
    Q_PROPERTY(QString searchIspdbFoundMessage MEMBER m_searchIspdbFoundMessage NOTIFY searchIspdbFoundMessageChanged)
    Q_PROPERTY(QString searchIspdbErrorMessage MEMBER m_searchIspdbErrorMessage NOTIFY searchIspdbErrorMessageChanged)

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

    Q_INVOKABLE void searchConfiguration();

Q_SIGNALS:
    void fullNameChanged();
    void emailChanged();
    void passwordChanged();
    void searchIspdbFoundMessageChanged();
    void searchIspdbErrorMessageChanged();

private:
    void setEmailProvider(const EmailProvider &emailProvider, const QString &messageInfo);
    void setErrorOccured(const QString &errorMessage);
    void clearConfiguration();
    Identity *const m_identity;
    IspdbService *const m_ispdbService;
    ConfigurationModel *const m_configurationModel;
    QString m_password;
    QString m_searchIspdbFoundMessage;
    QString m_searchIspdbErrorMessage;
};
