/*
    SPDX-FileCopyrightText: 2023-2024 Laurent Montel <montel.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/
#pragma once

#include "resource.h"
#include <KIMAP/LoginJob>
#include <KIdentityManagementCore/Identity>
#include <MailTransport/Transport>
#include <QDebug>
#include <QObject>
#include <QtQmlIntegration/qqmlintegration.h>

class ServerTest;

class ManualConfiguration : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    /// This property holds the mail transport configuration.
    Q_PROPERTY(MailTransport::Transport *mailTransport READ mailTransport CONSTANT FINAL)

    /// This property holds the identity configuration.
    Q_PROPERTY(KIdentityManagementCore::Identity identity READ identity WRITE setIdentity NOTIFY identityChanged FINAL)

    /// This property holds the password of the user.
    ///
    /// This will be used for both the IMAP/SMTP resource as well as the mail transport.
    Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged FINAL)

    /// This property holds the email of the user.
    Q_PROPERTY(QString email READ email WRITE setEmail NOTIFY emailChanged FINAL)

    Q_PROPERTY(QString incomingHostName READ incomingHostName WRITE setIncomingHostName NOTIFY incomingHostNameChanged FINAL)
    Q_PROPERTY(uint incomingPort READ incomingPort WRITE setIncomingPort NOTIFY incomingPortChanged FINAL)
    Q_PROPERTY(QString incomingUserName READ incomingUserName WRITE setIncomingUserName NOTIFY incomingUserNameChanged FINAL)

    Q_PROPERTY(bool configurationIsValid MEMBER mConfigurationIsValid NOTIFY configurationIsValidChanged FINAL)
    Q_PROPERTY(IncomingProtocol incomingProtocol READ incomingProtocol WRITE setIncomingProtocol NOTIFY incomingProtocolChanged FINAL)
    Q_PROPERTY(KIMAP::LoginJob::EncryptionMode incomingSecurityProtocol READ incomingSecurityProtocol WRITE setIncomingSecurityProtocol NOTIFY
                   incomingSecurityProtocolChanged FINAL)
    Q_PROPERTY(KIMAP::LoginJob::AuthenticationMode incomingAuthenticationProtocol READ incomingAuthenticationProtocol WRITE setIncomingAuthenticationProtocol
                   NOTIFY incomingAuthenticationProtocolChanged FINAL)

    Q_PROPERTY(bool disconnectedModeEnabled READ disconnectedModeEnabled WRITE setDisconnectedModeEnabled NOTIFY disconnectedModeEnabledChanged FINAL)

    Q_PROPERTY(bool hasDisconnectedMode MEMBER mHasDisconnectedMode NOTIFY hasDisconnectedModeChanged FINAL)
    Q_PROPERTY(bool serverTestInProgress MEMBER mServerTestInProgress NOTIFY serverTestInProgressModeChanged FINAL)
public:
    enum IncomingProtocol {
        IMAP,
        POP3,
        KOLAB,
    };
    Q_ENUM(IncomingProtocol)

    explicit ManualConfiguration(QObject *parent = nullptr);
    ~ManualConfiguration() override;

    /// Save the mail transport and resource
    Q_INVOKABLE void save();

    [[nodiscard]] MailTransport::Transport *mailTransport() const;

    [[nodiscard]] KIdentityManagementCore::Identity identity() const;
    void setIdentity(const KIdentityManagementCore::Identity &identity);

    [[nodiscard]] QString incomingHostName() const;
    void setIncomingHostName(const QString &newIncomingHostName);

    [[nodiscard]] uint incomingPort() const;
    void setIncomingPort(uint newPort);

    [[nodiscard]] QString incomingUserName() const;
    void setIncomingUserName(const QString &newIncomingUserName);

    [[nodiscard]] IncomingProtocol incomingProtocol() const;
    void setIncomingProtocol(IncomingProtocol incomingProtocol);

    [[nodiscard]] KIMAP::LoginJob::EncryptionMode incomingSecurityProtocol() const;
    void setIncomingSecurityProtocol(KIMAP::LoginJob::EncryptionMode incomingSecurityProtocol);

    [[nodiscard]] KIMAP::LoginJob::AuthenticationMode incomingAuthenticationProtocol() const;
    void setIncomingAuthenticationProtocol(KIMAP::LoginJob::AuthenticationMode incomingAuthenticationProtocols);

    [[nodiscard]] bool disconnectedModeEnabled() const;
    void setDisconnectedModeEnabled(bool disconnectedMode);

    Q_INVOKABLE void checkServer();
    Q_INVOKABLE void checkConfiguration();

    QString email() const;
    void setEmail(const QString &email);

    QString password() const;
    void setPassword(const QString &newPassword);

Q_SIGNALS:
    void passwordChanged();
    void emailChanged();
    void identityChanged();

    void configurationIsValidChanged();

    void incomingHostNameChanged();
    void incomingPortChanged();
    void incomingUserNameChanged();

    void incomingProtocolChanged();
    void incomingSecurityProtocolChanged();
    void incomingAuthenticationProtocolChanged();

    void disconnectedModeEnabledChanged();

    void hasDisconnectedModeChanged();

    void error(const QString &msg);
    void info(const QString &msg);
    void finished(const QString &msg);

    void serverTestInProgressModeChanged();
    void serverTestDone();

protected:
    void generateResource(const Resource::ResourceInfo &info);

private:
    [[nodiscard]] Resource::ResourceInfo createPop3Resource() const;
    [[nodiscard]] Resource::ResourceInfo createImapResource() const;
    [[nodiscard]] Resource::ResourceInfo createKolabResource() const;
    [[nodiscard]] QString generateUniqueAccountName() const;
    void slotTestFail();
    void slotTestResult(const QString &result);

    // Incoming
    QString mIncomingUserName;
    QString mIncomingHostName;
    uint mIncomingPort = 995;

    QString mPassword;

    IncomingProtocol mIncomingProtocol = IMAP;
    KIMAP::LoginJob::EncryptionMode mIncomingSecurityProtocol = KIMAP::LoginJob::SSLorTLS;
    KIMAP::LoginJob::AuthenticationMode mIncomingAuthenticationProtocol = KIMAP::LoginJob::AuthenticationMode::Login;

    // configuration is Valid
    bool mConfigurationIsValid = false;

    // Has DisconnectedMode
    bool mHasDisconnectedMode = false;

    bool mDisconnectedModeEnabled = false;

    bool mServerTestInProgress = false;

    ServerTest *mServerTest = nullptr;

    MailTransport::Transport *const mMailTransport;
    mutable KIdentityManagementCore::Identity mIdentity;
};

QDebug operator<<(QDebug d, const ManualConfiguration &t);
