// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

#pragma once

#include <QObject>

class Identity;
class IspdbService;
class ConfigurationModel;

class SetupManager : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString email READ email WRITE setEmail NOTIFY emailChanged)
    Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged)
    Q_PROPERTY(QString fullName READ fullName WRITE setFullName NOTIFY fullNameChanged)
    Q_PROPERTY(Identity *identity READ identity CONSTANT)
    Q_PROPERTY(ConfigurationModel *configurationModel READ configurationModel CONSTANT)

public:
    explicit SetupManager(QObject *parent = nullptr);
    ~SetupManager();

    QString fullName() const;
    void setFullName(const QString &fullName);

    QString email() const;
    void setEmail(const QString &email);

    QString password() const;
    void setPassword(const QString &password);

    Identity *identity() const;
    ConfigurationModel *configurationModel() const;

    Q_INVOKABLE void searchConfiguration();

Q_SIGNALS:
    void fullNameChanged();
    void emailChanged();
    void passwordChanged();

private:
    Identity *const m_identity;
    IspdbService *const m_ispdbService;
    ConfigurationModel *const m_configurationModel;
    QString m_password;
};
