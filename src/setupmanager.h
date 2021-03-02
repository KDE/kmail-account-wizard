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

class SetupObject;
class SetupPage;

class SetupManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString email READ email WRITE setEmail NOTIFY emailChanged)
    Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged)
    Q_PROPERTY(QString country READ country CONSTANT)
    Q_PROPERTY(bool personalDataAvailable READ personalDataAvailable WRITE setPersonalDataAvailable NOTIFY personalDataAvailableChanged)

public:
    explicit SetupManager(QObject *parent = nullptr);
    ~SetupManager() override;
    void setSetupPage(SetupPage *page);

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

public Q_SLOTS:
    /** Ensures the wallet is open for subsequent sync wallet access in the resources. */
    void openWallet();
    QObject *createResource(const QString &type);
    QObject *createTransport(const QString &type);
    QObject *createConfigFile(const QString &configName);
    QObject *createLdap();
    QObject *createIdentity();
    QObject *createKey();
    void execute();
    void setupInfo(const QString &msg);
    QObject *ispDB(const QString &type);

    void requestRollback();

Q_SIGNALS:
    void rollbackComplete();
    void setupFinished(SetupObject *obj);
    void nameChanged();
    void emailChanged();
    void passwordChanged();
    void personalDataAvailableChanged();

private:
    void setupNext();
    void rollback();
    SetupObject *connectObject(SetupObject *obj);

private Q_SLOTS:
    void setupSucceeded(const QString &msg);
    void setupFailed(const QString &msg);

private:
    QString m_name, m_email, m_password;
    QVector<SetupObject *> m_objectToSetup;
    QVector<SetupObject *> m_setupObjects;
    SetupObject *m_currentSetupObject = nullptr;
    SetupPage *m_page = nullptr;
    KWallet::Wallet *m_wallet = nullptr;
    GpgME::Key m_key;
    Key::PublishingMethod m_keyPublishingMethod;
    bool m_personalDataAvailable = false;
    bool m_rollbackRequested = false;
    bool m_pgpAutoSign = false;
    bool m_pgpAutoEncrypt = false;
};

