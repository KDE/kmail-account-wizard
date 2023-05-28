/*
    SPDX-FileCopyrightText: 2023 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "manualconfiguration.h"
#include <KLocalizedString>

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
