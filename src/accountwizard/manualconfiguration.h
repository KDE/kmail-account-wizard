/*
    SPDX-FileCopyrightText: 2023 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/
#pragma once

#include <QDebug>
#include <QObject>

class ManualConfiguration : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString incomingHostName READ incomingHostName WRITE setIncomingHostName NOTIFY incomingHostNameChanged FINAL)
    Q_PROPERTY(int incomingPort READ incomingPort WRITE setIncomingPort NOTIFY incomingPortChanged FINAL)
    Q_PROPERTY(QString incomingUserName READ incomingUserName WRITE setIncomingUserName NOTIFY incomingUserNameChanged FINAL)

    Q_PROPERTY(QString outgoingHostName READ outgoingHostName WRITE setOutgoingHostName NOTIFY outgoingHostNameChanged FINAL)
    Q_PROPERTY(int outgoingPort READ outgoingPort WRITE setOutgoingPort NOTIFY outgoingPortChanged FINAL)
    Q_PROPERTY(QString outgoingUserName READ outgoingUserName WRITE setOutgoingUserName NOTIFY outgoingUserNameChanged FINAL)
    Q_PROPERTY(QStringList incomingProtocols READ incomingProtocols CONSTANT)
    Q_PROPERTY(QStringList securityProtocols READ securityProtocols CONSTANT)
    Q_PROPERTY(QStringList authenticationProtocols READ authenticationProtocols CONSTANT)
    Q_PROPERTY(bool configurationIsValid MEMBER mConfigurationIsValid NOTIFY configurationIsValidChanged)
    Q_PROPERTY(int currentIncomingProtocol READ currentIncomingProtocol WRITE setCurrentIncomingProtocol NOTIFY currentIncomingProtocolChanged FINAL)
    Q_PROPERTY(int currentIncomingSecurityProtocol READ currentIncomingSecurityProtocol WRITE setCurrentIncomingSecurityProtocol NOTIFY
                   currentIncomingSecurityProtocolChanged FINAL)
    Q_PROPERTY(int currentOutgoingSecurityProtocol READ currentOutgoingSecurityProtocol WRITE setCurrentOutgoingSecurityProtocol NOTIFY
                   currentOutgoingSecurityProtocolChanged FINAL)
    Q_PROPERTY(int currentIncomingAuthenticationProtocols READ currentIncomingAuthenticationProtocols WRITE setCurrentIncomingAuthenticationProtocols NOTIFY
                   currentIncomingAuthenticationProtocolsChanged FINAL)
    Q_PROPERTY(int currentOutgoingAuthenticationProtocols READ currentOutgoingAuthenticationProtocols WRITE setCurrentOutgoingAuthenticationProtocols NOTIFY
                   currentOutgoingAuthenticationProtocolsChanged FINAL)

public:
    explicit ManualConfiguration(QObject *parent = nullptr);
    ~ManualConfiguration() override;

    Q_REQUIRED_RESULT QString incomingHostName() const;
    void setIncomingHostName(const QString &newIncomingHostName);

    Q_REQUIRED_RESULT int incomingPort() const;
    void setIncomingPort(int newPort);

    Q_REQUIRED_RESULT QString incomingUserName() const;
    void setIncomingUserName(const QString &newIncomingUserName);

    Q_REQUIRED_RESULT QString outgoingHostName() const;
    void setOutgoingHostName(const QString &newOutgoingHostName);

    Q_REQUIRED_RESULT int outgoingPort() const;
    void setOutgoingPort(int newPort);

    Q_REQUIRED_RESULT QString outgoingUserName() const;
    void setOutgoingUserName(const QString &newOutgoingUserName);

    Q_REQUIRED_RESULT QStringList incomingProtocols() const;
    Q_REQUIRED_RESULT QStringList securityProtocols() const;
    Q_REQUIRED_RESULT QStringList authenticationProtocols() const;

    Q_REQUIRED_RESULT int currentIncomingProtocol() const;
    void setCurrentIncomingProtocol(int newCurrentIncomingProtocol);

    Q_REQUIRED_RESULT int currentIncomingSecurityProtocol() const;
    void setCurrentIncomingSecurityProtocol(int newCurrentIncomingSecurityProtocol);

    Q_REQUIRED_RESULT int currentOutgoingSecurityProtocol() const;
    void setCurrentOutgoingSecurityProtocol(int newCurrentOutgoingSecurityProtocol);

    Q_REQUIRED_RESULT int currentIncomingAuthenticationProtocols() const;
    void setCurrentIncomingAuthenticationProtocols(int newCurrentIncomingAuthenticationProtocols);

    Q_REQUIRED_RESULT int currentOutgoingAuthenticationProtocols() const;
    void setCurrentOutgoingAuthenticationProtocols(int newCurrentOutgoingAuthenticationProtocols);

    Q_INVOKABLE void checkServer();

    void createManualAccount();

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

    void currentIncomingAuthenticationProtocolsChanged();
    void currentOutgoingAuthenticationProtocolsChanged();

private:
    void checkConfiguration();

    // Incoming
    QString mIncomingUserName;
    QString mIncomingHostName;
    int mIncomingPort = 995;

    // Outgoing
    QString mOutgoingUserName;
    QString mOutgoingHostName;
    int mOutgoingPort = 465;

    int mCurrentIncomingProtocol = 0; // POP3

    int mCurrentIncomingSecurityProtocol = 2; // NONE
    int mCurrentOutgoingSecurityProtocol = 2; // NONE

    int mCurrentIncomingAuthenticationProtocols = 0;
    int mCurrentOutgoingAuthenticationProtocols = 0;

    // configuration is Valid
    bool mConfigurationIsValid = false;
};
QDebug operator<<(QDebug d, const ManualConfiguration &t);
