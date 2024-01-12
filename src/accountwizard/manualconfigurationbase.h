/*
    SPDX-FileCopyrightText: 2023-2024 Laurent Montel <montel.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/
#pragma once

#include "libaccountwizard_export.h"
#include "resource.h"
#include "transport.h"
#include <QDebug>
#include <QObject>
class ServerTest;
class LIBACCOUNTWIZARD_EXPORT ManualConfigurationBase : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString incomingHostName READ incomingHostName WRITE setIncomingHostName NOTIFY incomingHostNameChanged FINAL)
    Q_PROPERTY(uint incomingPort READ incomingPort WRITE setIncomingPort NOTIFY incomingPortChanged FINAL)
    Q_PROPERTY(QString incomingUserName READ incomingUserName WRITE setIncomingUserName NOTIFY incomingUserNameChanged FINAL)

    Q_PROPERTY(QString outgoingHostName READ outgoingHostName WRITE setOutgoingHostName NOTIFY outgoingHostNameChanged FINAL)
    Q_PROPERTY(int outgoingPort READ outgoingPort WRITE setOutgoingPort NOTIFY outgoingPortChanged FINAL)
    Q_PROPERTY(QString outgoingUserName READ outgoingUserName WRITE setOutgoingUserName NOTIFY outgoingUserNameChanged FINAL)
    Q_PROPERTY(QStringList incomingProtocols READ incomingProtocols CONSTANT)
    Q_PROPERTY(QStringList securityProtocols READ securityProtocols CONSTANT)
    Q_PROPERTY(QStringList authenticationProtocols READ authenticationProtocols CONSTANT)
    Q_PROPERTY(bool configurationIsValid MEMBER mConfigurationIsValid NOTIFY configurationIsValidChanged FINAL)
    Q_PROPERTY(int currentIncomingProtocol READ currentIncomingProtocol WRITE setCurrentIncomingProtocol NOTIFY currentIncomingProtocolChanged FINAL)
    Q_PROPERTY(int currentIncomingSecurityProtocol READ currentIncomingSecurityProtocol WRITE setCurrentIncomingSecurityProtocol NOTIFY
                   currentIncomingSecurityProtocolChanged FINAL)
    Q_PROPERTY(int currentOutgoingSecurityProtocol READ currentOutgoingSecurityProtocol WRITE setCurrentOutgoingSecurityProtocol NOTIFY
                   currentOutgoingSecurityProtocolChanged FINAL)
    Q_PROPERTY(int currentIncomingAuthenticationProtocol READ currentIncomingAuthenticationProtocol WRITE setCurrentIncomingAuthenticationProtocol NOTIFY
                   currentIncomingAuthenticationProtocolChanged FINAL)
    Q_PROPERTY(int currentOutgoingAuthenticationProtocol READ currentOutgoingAuthenticationProtocol WRITE setCurrentOutgoingAuthenticationProtocol NOTIFY
                   currentOutgoingAuthenticationProtocolChanged FINAL)

    Q_PROPERTY(bool disconnectedModeEnabled READ disconnectedModeEnabled WRITE setDisconnectedModeEnabled NOTIFY disconnectedModeEnabledChanged FINAL)

    Q_PROPERTY(bool hasDisconnectedMode MEMBER mHasDisconnectedMode NOTIFY hasDisconnectedModeChanged FINAL)
    Q_PROPERTY(bool serverTestInProgress MEMBER mServerTestInProgress NOTIFY serverTestInProgressModeChanged FINAL)
public:
    explicit ManualConfigurationBase(QObject *parent = nullptr);
    ~ManualConfigurationBase() override;

    [[nodiscard]] QString incomingHostName() const;
    void setIncomingHostName(const QString &newIncomingHostName);

    [[nodiscard]] uint incomingPort() const;
    void setIncomingPort(uint newPort);

    [[nodiscard]] QString incomingUserName() const;
    void setIncomingUserName(const QString &newIncomingUserName);

    [[nodiscard]] QString outgoingHostName() const;
    void setOutgoingHostName(const QString &newOutgoingHostName);

    [[nodiscard]] int outgoingPort() const;
    void setOutgoingPort(int newPort);

    [[nodiscard]] QString outgoingUserName() const;
    void setOutgoingUserName(const QString &newOutgoingUserName);

    [[nodiscard]] QStringList incomingProtocols() const;
    [[nodiscard]] QStringList securityProtocols() const;
    [[nodiscard]] QStringList authenticationProtocols() const;

    [[nodiscard]] int currentIncomingProtocol() const;
    void setCurrentIncomingProtocol(int newCurrentIncomingProtocol);

    [[nodiscard]] int currentIncomingSecurityProtocol() const;
    void setCurrentIncomingSecurityProtocol(int newCurrentIncomingSecurityProtocol);

    [[nodiscard]] int currentOutgoingSecurityProtocol() const;
    void setCurrentOutgoingSecurityProtocol(int newCurrentOutgoingSecurityProtocol);

    [[nodiscard]] int currentIncomingAuthenticationProtocol() const;
    void setCurrentIncomingAuthenticationProtocol(int newCurrentIncomingAuthenticationProtocols);

    [[nodiscard]] int currentOutgoingAuthenticationProtocol() const;
    void setCurrentOutgoingAuthenticationProtocol(int newCurrentOutgoingAuthenticationProtocols);

    [[nodiscard]] bool disconnectedModeEnabled() const;
    void setDisconnectedModeEnabled(bool disconnectedMode);

    Q_INVOKABLE void checkServer();

    void createManualAccount();

    void setEmail(const QString &email);

    void setPassword(const QString &newPassword);

    void setIdentityId(int id);

    int identityId() const;

Q_SIGNALS:
    void incomingHostNameChanged();
    void incomingPortChanged();
    void incomingUserNameChanged();

    void outgoingHostNameChanged();
    void outgoingPortChanged();
    void outgoingUserNameChanged();

    void configurationIsValidChanged();
    void currentIncomingProtocolChanged();

    void currentIncomingSecurityProtocolChanged();
    void currentOutgoingSecurityProtocolChanged();

    void currentIncomingAuthenticationProtocolChanged();
    void currentOutgoingAuthenticationProtocolChanged();

    void disconnectedModeEnabledChanged();

    void hasDisconnectedModeChanged();

    void error(const QString &msg);
    void info(const QString &msg);
    void finished(const QString &msg);

    void serverTestInProgressModeChanged();
    void serverTestDone();

protected:
    LIBACCOUNTWIZARD_NO_EXPORT virtual void createTransport();
    [[nodiscard]] LIBACCOUNTWIZARD_NO_EXPORT Transport::TransportInfo createTransportInfo() const;
    LIBACCOUNTWIZARD_NO_EXPORT virtual void generateResource(const Resource::ResourceInfo &info);

private:
    LIBACCOUNTWIZARD_NO_EXPORT void createResource();
    [[nodiscard]] LIBACCOUNTWIZARD_NO_EXPORT Resource::ResourceInfo createPop3Resource() const;
    [[nodiscard]] LIBACCOUNTWIZARD_NO_EXPORT Resource::ResourceInfo createImapResource() const;
    [[nodiscard]] LIBACCOUNTWIZARD_NO_EXPORT Resource::ResourceInfo createKolabResource() const;
    LIBACCOUNTWIZARD_NO_EXPORT void checkConfiguration();
    [[nodiscard]] LIBACCOUNTWIZARD_NO_EXPORT QString convertOutgoingSecurityProtocol(int protocol) const;
    [[nodiscard]] LIBACCOUNTWIZARD_NO_EXPORT QString convertOutgoingAuthenticationProtocol(int protocol) const;
    [[nodiscard]] LIBACCOUNTWIZARD_NO_EXPORT QString generateUniqueAccountName() const;
    [[nodiscard]] LIBACCOUNTWIZARD_NO_EXPORT QString convertIncomingAuthenticationProtocol(int protocol) const;
    [[nodiscard]] LIBACCOUNTWIZARD_NO_EXPORT QString convertIncomingSecurityProtocol(int index) const;
    LIBACCOUNTWIZARD_NO_EXPORT void slotTestFail();
    LIBACCOUNTWIZARD_NO_EXPORT void slotTestResult(const QString &result);

    // Incoming
    QString mIncomingUserName;
    QString mIncomingHostName;
    uint mIncomingPort = 995;

    // Outgoing
    QString mOutgoingUserName;
    QString mOutgoingHostName;

    QString mPassword;

    int mOutgoingPort = 465;

    int mCurrentIncomingProtocol = 0; // POP3

    int mCurrentIncomingSecurityProtocol = 2; // NONE
    int mCurrentOutgoingSecurityProtocol = 2; // NONE

    int mCurrentIncomingAuthenticationProtocol = 0; // Clear Text
    int mCurrentOutgoingAuthenticationProtocol = 0; // Clear Text

    int mIdentityId = 0;

    // configuration is Valid
    bool mConfigurationIsValid = false;

    // Has DisconnectedMode
    bool mHasDisconnectedMode = false;

    bool mDisconnectedModeEnabled = false;

    bool mServerTestInProgress = false;

    ServerTest *mServerTest = nullptr;
};
QDebug operator<<(QDebug d, const ManualConfigurationBase &t);
