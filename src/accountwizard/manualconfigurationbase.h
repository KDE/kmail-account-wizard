/*
    SPDX-FileCopyrightText: 2023-2024 Laurent Montel <montel.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/
#pragma once

#include "libaccountwizard_export.h"
#include "resource.h"
#include <KIMAP/LoginJob>
#include <MailTransport/Transport>
#include <QDebug>
#include <QObject>

class ServerTest;

class LIBACCOUNTWIZARD_EXPORT ManualConfigurationBase : public QObject
{
    Q_OBJECT

    /// This property holds the mail transport configuration.
    Q_PROPERTY(MailTransport::Transport *mailTransport READ mailTransport CONSTANT)

    Q_PROPERTY(QString incomingHostName READ incomingHostName WRITE setIncomingHostName NOTIFY incomingHostNameChanged FINAL)
    Q_PROPERTY(uint incomingPort READ incomingPort WRITE setIncomingPort NOTIFY incomingPortChanged FINAL)
    Q_PROPERTY(QString incomingUserName READ incomingUserName WRITE setIncomingUserName NOTIFY incomingUserNameChanged FINAL)

    Q_PROPERTY(QStringList incomingProtocols READ incomingProtocols CONSTANT)
    Q_PROPERTY(QStringList securityProtocols READ securityProtocols CONSTANT)
    Q_PROPERTY(bool configurationIsValid MEMBER mConfigurationIsValid NOTIFY configurationIsValidChanged FINAL)
    Q_PROPERTY(int currentIncomingProtocol READ currentIncomingProtocol WRITE setCurrentIncomingProtocol NOTIFY currentIncomingProtocolChanged FINAL)
    Q_PROPERTY(int currentIncomingSecurityProtocol READ currentIncomingSecurityProtocol WRITE setCurrentIncomingSecurityProtocol NOTIFY
                   currentIncomingSecurityProtocolChanged FINAL)
    Q_PROPERTY(KIMAP::LoginJob::AuthenticationMode currentIncomingAuthenticationProtocol READ currentIncomingAuthenticationProtocol WRITE
                   setCurrentIncomingAuthenticationProtocol NOTIFY currentIncomingAuthenticationProtocolChanged FINAL)

    Q_PROPERTY(bool disconnectedModeEnabled READ disconnectedModeEnabled WRITE setDisconnectedModeEnabled NOTIFY disconnectedModeEnabledChanged FINAL)

    Q_PROPERTY(bool hasDisconnectedMode MEMBER mHasDisconnectedMode NOTIFY hasDisconnectedModeChanged FINAL)
    Q_PROPERTY(bool serverTestInProgress MEMBER mServerTestInProgress NOTIFY serverTestInProgressModeChanged FINAL)
public:
    explicit ManualConfigurationBase(QObject *parent = nullptr);
    ~ManualConfigurationBase() override;

    /// Save the mail transport and resource
    Q_INVOKABLE void save();

    [[nodiscard]] MailTransport::Transport *mailTransport() const;

    [[nodiscard]] QString incomingHostName() const;
    void setIncomingHostName(const QString &newIncomingHostName);

    [[nodiscard]] uint incomingPort() const;
    void setIncomingPort(uint newPort);

    [[nodiscard]] QString incomingUserName() const;
    void setIncomingUserName(const QString &newIncomingUserName);

    [[nodiscard]] QStringList incomingProtocols() const;
    [[nodiscard]] QStringList securityProtocols() const;

    [[nodiscard]] int currentIncomingProtocol() const;
    void setCurrentIncomingProtocol(int newCurrentIncomingProtocol);

    [[nodiscard]] int currentIncomingSecurityProtocol() const;
    void setCurrentIncomingSecurityProtocol(int newCurrentIncomingSecurityProtocol);

    [[nodiscard]] KIMAP::LoginJob::AuthenticationMode currentIncomingAuthenticationProtocol() const;
    void setCurrentIncomingAuthenticationProtocol(KIMAP::LoginJob::AuthenticationMode newCurrentIncomingAuthenticationProtocols);

    [[nodiscard]] bool disconnectedModeEnabled() const;
    void setDisconnectedModeEnabled(bool disconnectedMode);

    Q_INVOKABLE void checkServer();

    void setEmail(const QString &email);

    void setPassword(const QString &newPassword);

    void setIdentityId(int id);

    int identityId() const;

Q_SIGNALS:
    void incomingHostNameChanged();
    void incomingPortChanged();
    void incomingUserNameChanged();

    void configurationIsValidChanged();
    void currentIncomingProtocolChanged();

    void currentIncomingSecurityProtocolChanged();

    void currentIncomingAuthenticationProtocolChanged();

    void disconnectedModeEnabledChanged();

    void hasDisconnectedModeChanged();

    void error(const QString &msg);
    void info(const QString &msg);
    void finished(const QString &msg);

    void serverTestInProgressModeChanged();
    void serverTestDone();

protected:
    LIBACCOUNTWIZARD_NO_EXPORT virtual void generateResource(const Resource::ResourceInfo &info);

private:
    [[nodiscard]] LIBACCOUNTWIZARD_NO_EXPORT Resource::ResourceInfo createPop3Resource() const;
    [[nodiscard]] LIBACCOUNTWIZARD_NO_EXPORT Resource::ResourceInfo createImapResource() const;
    [[nodiscard]] LIBACCOUNTWIZARD_NO_EXPORT Resource::ResourceInfo createKolabResource() const;
    LIBACCOUNTWIZARD_NO_EXPORT void checkConfiguration();
    [[nodiscard]] LIBACCOUNTWIZARD_NO_EXPORT QString generateUniqueAccountName() const;
    [[nodiscard]] LIBACCOUNTWIZARD_NO_EXPORT QString convertIncomingSecurityProtocol(int index) const;
    LIBACCOUNTWIZARD_NO_EXPORT void slotTestFail();
    LIBACCOUNTWIZARD_NO_EXPORT void slotTestResult(const QString &result);

    // Incoming
    QString mIncomingUserName;
    QString mIncomingHostName;
    uint mIncomingPort = 995;

    QString mPassword;

    int mCurrentIncomingProtocol = 0; // POP3

    int mCurrentIncomingSecurityProtocol = 2; // NONE

    KIMAP::LoginJob::AuthenticationMode mCurrentIncomingAuthenticationProtocol = KIMAP::LoginJob::AuthenticationMode::ClearText;

    int mIdentityId = 0;

    // configuration is Valid
    bool mConfigurationIsValid = false;

    // Has DisconnectedMode
    bool mHasDisconnectedMode = false;

    bool mDisconnectedModeEnabled = false;

    bool mServerTestInProgress = false;

    ServerTest *mServerTest = nullptr;

    MailTransport::Transport *const mMailTransport;
};

QDebug operator<<(QDebug d, const ManualConfigurationBase &t);
