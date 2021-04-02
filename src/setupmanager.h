/*
    SPDX-FileCopyrightText: 2009 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "key.h"
#include <QObject>
#include <QVector>
#include <gpgme++/key.h>

namespace KWallet
{
class Wallet;
}

class Ldap;
class ConfigFile;
class Key;
class Identity;
class Transport;
class Resource;
class IspdbHelper;

/// Controller of the account wizard
class SetupManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString email READ email WRITE setEmail NOTIFY emailChanged)
    Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged)
    Q_PROPERTY(QString country READ country CONSTANT)
    Q_PROPERTY(bool personalDataAvailable READ personalDataAvailable WRITE setPersonalDataAvailable NOTIFY personalDataAvailableChanged)

public:
    static SetupManager &instance();

    void setName(const QString &);
    void setEmail(const QString &);
    void setPassword(const QString &);
    void setPersonalDataAvailable(bool available);
    void setPgpAutoSign(bool autosign);
    void setPgpAutoEncrypt(bool autoencrypt);
    void setKey(const GpgME::Key &key);
    void setKeyPublishingMethod(Key::PublishingMethod method);

    QVector<SetupObject *> objectsToSetup() const;
    QVector<SetupObject *> setupObjects() const;

    bool personalDataAvailable() const;
    QString name() const;
    QString email() const;
    QString password() const;
    QString country() const;

    /// Ensures the wallet is open for subsequent sync wallet access in the resources.
    Q_INVOKABLE void openWallet();
    ///
    Q_INVOKABLE Resource *createResource(const QString &type);
    Q_INVOKABLE Transport *createTransport(const QString &type);
    Q_INVOKABLE ConfigFile *createConfigFile(const QString &configName);
    Q_INVOKABLE Ldap *createLdap();
    Q_INVOKABLE Identity *createIdentity();
    Q_INVOKABLE Key *createKey();
    Q_INVOKABLE void execute();
    Q_INVOKABLE void setupInfo(const QString &msg);
    Q_INVOKABLE IspdbHelper *ispDB(const QString &type);

    Q_INVOKABLE void requestRollback();

Q_SIGNALS:
    void rollbackComplete();
    void nameChanged();
    void emailChanged();
    void passwordChanged();
    void personalDataAvailableChanged();

private:
    explicit SetupManager(QObject *parent = nullptr);
    ~SetupManager() override;
    void setupNext();
    void rollback();

private Q_SLOTS:
    void setupSucceeded(const QString &msg);
    void setupFailed(const QString &msg);

private:
    QString m_name;
    QString m_email;
    QString m_password;
    QVector<SetupObject *> m_objectToSetup;
    QVector<SetupObject *> m_setupObjects;
    SetupObject *m_currentSetupObject = nullptr;
    KWallet::Wallet *m_wallet = nullptr;
    GpgME::Key m_key;
    Key::PublishingMethod m_keyPublishingMethod;
    bool m_personalDataAvailable = false;
    bool m_rollbackRequested = false;
    bool m_pgpAutoSign = false;
    bool m_pgpAutoEncrypt = false;
};

