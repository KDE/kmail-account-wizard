// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

#pragma once
#include "ispdb/configurationmodel.h"
#include <QObject>

class IspdbService;

class SetupManager : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_PROPERTY(QString email READ email WRITE setEmail NOTIFY emailChanged)
    Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged)
    Q_PROPERTY(QString fullName READ fullName WRITE setFullName NOTIFY fullNameChanged)
    Q_PROPERTY(ConfigurationModel *configurationModel READ configurationModel CONSTANT)
    Q_PROPERTY(QString searchIspdbFoundMessage MEMBER mSearchIspdbFoundMessage NOTIFY searchIspdbFoundMessageChanged)
    Q_PROPERTY(bool noConfigFound MEMBER mNoConfigFound NOTIFY noConfigFoundChanged)
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

    ConfigurationModel *configurationModel() const;

    Q_INVOKABLE void searchConfiguration();

    [[nodiscard]] QString details() const;

Q_SIGNALS:
    void fullNameChanged();
    void emailChanged();
    void passwordChanged();
    void searchIspdbFoundMessageChanged();
    void noConfigFoundChanged();
    void detailsChanged();

private:
    void setEmailProvider(const EmailProvider &emailProvider, const QString &messageInfo);
    void clearConfiguration();
    void noConfigFound();
    void slotError(const QString &str);
    void slotFinished(const QString &str);
    void slotInfo(const QString &str);
    QString mPassword;
    QString mEmail;
    QString mFullName;
    QString mSearchIspdbFoundMessage;
    QString mDetails;
    IspdbService *const mIspdbService;
    ConfigurationModel *const mConfigurationModel;
    bool mNoConfigFound = false;
};
