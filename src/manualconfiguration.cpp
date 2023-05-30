/*
    SPDX-FileCopyrightText: 2023 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "manualconfiguration.h"
#include <KLocalizedString>
#include <QDebug>

ManualConfiguration::ManualConfiguration(QObject *parent)
    : QObject{parent}
{
}

ManualConfiguration::~ManualConfiguration() = default;

QStringList ManualConfiguration::incomingProtocols() const
{
    return {i18n("POP3"), i18n("IMAP")};
}

QStringList ManualConfiguration::securityProtocols() const
{
    return {i18n("STARTTLS"), i18n("SSL/TLS"), i18n("None")};
}

QStringList ManualConfiguration::authenticationProtocols() const
{
    return {i18n("Clear text"), i18n("LOGIN"), i18n("PLAIN"), i18n("CRAM-MD5"), i18n("DIGEST-MD5"), i18n("NTLM"), i18n("GSSAPI")};
}

void ManualConfiguration::createManualAccount()
{
    qDebug() << " createManualAccount ";
    // TODO
}

void ManualConfiguration::checkServer()
{
    qDebug() << " Verify server";
}

int ManualConfiguration::currentOutgoingAuthenticationProtocols() const
{
    return mCurrentOutgoingAuthenticationProtocols;
}

void ManualConfiguration::setCurrentOutgoingAuthenticationProtocols(int newCurrentOutgoingAuthenticationProtocols)
{
    if (mCurrentOutgoingAuthenticationProtocols == newCurrentOutgoingAuthenticationProtocols)
        return;
    mCurrentOutgoingAuthenticationProtocols = newCurrentOutgoingAuthenticationProtocols;
    qDebug() << " setCurrentOutgoingAuthenticationProtocols " << mCurrentOutgoingAuthenticationProtocols;
    Q_EMIT currentOutgoingAuthenticationProtocolsChanged();
}

int ManualConfiguration::currentIncomingAuthenticationProtocols() const
{
    return mCurrentIncomingAuthenticationProtocols;
}

void ManualConfiguration::setCurrentIncomingAuthenticationProtocols(int newCurrentIncomingAuthenticationProtocols)
{
    if (mCurrentIncomingAuthenticationProtocols == newCurrentIncomingAuthenticationProtocols)
        return;
    mCurrentIncomingAuthenticationProtocols = newCurrentIncomingAuthenticationProtocols;
    qDebug() << " setCurrentIncomingAuthenticationProtocols " << mCurrentIncomingAuthenticationProtocols;
    Q_EMIT currentIncomingAuthenticationProtocolsChanged();
}

int ManualConfiguration::currentOutgoingSecurityProtocol() const
{
    return mCurrentOutgoingSecurityProtocol;
}

void ManualConfiguration::setCurrentOutgoingSecurityProtocol(int newCurrentOutgoingSecurityProtocol)
{
    if (mCurrentOutgoingSecurityProtocol == newCurrentOutgoingSecurityProtocol)
        return;
    mCurrentOutgoingSecurityProtocol = newCurrentOutgoingSecurityProtocol;
    qDebug() << " setCurrentOutgoingSecurityProtocol " << mCurrentOutgoingSecurityProtocol;
    Q_EMIT currentOutgoingSecurityProtocolChanged();
}

int ManualConfiguration::currentIncomingSecurityProtocol() const
{
    return mCurrentIncomingSecurityProtocol;
}

void ManualConfiguration::setCurrentIncomingSecurityProtocol(int newCurrentIncomingSecurityProtocol)
{
    if (mCurrentIncomingSecurityProtocol == newCurrentIncomingSecurityProtocol)
        return;
    mCurrentIncomingSecurityProtocol = newCurrentIncomingSecurityProtocol;
    qDebug() << " setCurrentIncomingSecurityProtocol " << mCurrentIncomingSecurityProtocol;
    Q_EMIT currentIncomingSecurityProtocolChanged();
}

void ManualConfiguration::setCurrentIncomingProtocol(int newCurrentIncomingProtocol)
{
    if (mCurrentIncomingProtocol != newCurrentIncomingProtocol) {
        mCurrentIncomingProtocol = newCurrentIncomingProtocol;
        qDebug() << " setCurrentIncomingProtocol " << mCurrentIncomingProtocol;
        Q_EMIT currentIncomingProtocolChanged();
    }
}

int ManualConfiguration::currentIncomingProtocol() const
{
    return mCurrentIncomingProtocol;
}

QString ManualConfiguration::incomingHostName() const
{
    return mIncomingHostName;
}

void ManualConfiguration::setIncomingHostName(const QString &newIncomingHostName)
{
    if (mIncomingHostName != newIncomingHostName) {
        mIncomingHostName = newIncomingHostName;
        Q_EMIT incomingHostNameChanged();
    }
}

int ManualConfiguration::incomingPort() const
{
    return mIncomingPort;
}

void ManualConfiguration::setIncomingPort(int newPort)
{
    if (mIncomingPort != newPort) {
        mIncomingPort = newPort;
        Q_EMIT incomingPortChanged();
    }
}

QString ManualConfiguration::incomingUserName() const
{
    return mIncomingUserName;
}

void ManualConfiguration::setIncomingUserName(const QString &newIncomingUserName)
{
    if (mIncomingUserName != newIncomingUserName) {
        mIncomingUserName = newIncomingUserName;
        Q_EMIT incomingUserNameChanged();
    }
}

QString ManualConfiguration::outgoingHostName() const
{
    return mOutgoingHostName;
}

void ManualConfiguration::setOutgoingHostName(const QString &newOutgoingHostName)
{
    if (mOutgoingHostName != newOutgoingHostName) {
        mOutgoingHostName = newOutgoingHostName;
        Q_EMIT outgoingHostNameChanged();
    }
}

int ManualConfiguration::outgoingPort() const
{
    return mOutgoingPort;
}

void ManualConfiguration::setOutgoingPort(int newPort)
{
    if (mOutgoingPort != newPort) {
        mOutgoingPort = newPort;
        Q_EMIT outgoingPortChanged();
    }
}

QString ManualConfiguration::outgoingUserName() const
{
    return mOutgoingUserName;
}

void ManualConfiguration::setOutgoingUserName(const QString &newOutgoingUserName)
{
    if (mOutgoingUserName != newOutgoingUserName) {
        mOutgoingUserName = newOutgoingUserName;
        Q_EMIT outgoingUserNameChanged();
    }
}
