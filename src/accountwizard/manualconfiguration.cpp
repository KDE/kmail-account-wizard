/*
    SPDX-FileCopyrightText: 2023 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "manualconfiguration.h"
#include "resource.h"
#include "transport.h"
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

void ManualConfiguration::createResource()
{
    // Create incoming account
    QString resourceType;
    qDebug() << " createManualAccount ";
    if (mCurrentIncomingProtocol == 0) { // POP3
        resourceType = QStringLiteral("akonadi_pop3_resource");
    } else if (mCurrentIncomingProtocol == 1) { // IMAP
        resourceType = QStringLiteral("akonadi_imap_resource");
    } else {
        qDebug() << " invalid protocol ";
        return;
    }
    auto resource = new Resource(resourceType, this);
    // TODO connect
    resource->createResource();
}

void ManualConfiguration::createTransport()
{
    // Create outgoing account
    // TODO
    auto transport = new Transport(QString() /* TODO */, this);
    transport->setTransportInfo(createTransportInfo());
    // TODO connect
    transport->createTransport();
}

void ManualConfiguration::createManualAccount()
{
    createResource();
    createTransport();
}

Transport::TransportInfo ManualConfiguration::createTransportInfo() const
{
    Transport::TransportInfo info;
    // TODO
    return info;
}

void ManualConfiguration::checkServer()
{
    qDebug() << " Verify server";
}

void ManualConfiguration::checkConfiguration()
{
    const bool valid = !mIncomingUserName.trimmed().isEmpty() && !mIncomingHostName.trimmed().isEmpty() && !mOutgoingHostName.trimmed().isEmpty()
        && !mOutgoingUserName.trimmed().isEmpty();
    mConfigurationIsValid = valid;
    Q_EMIT configurationIsValidChanged();
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
    checkConfiguration();
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
    checkConfiguration();
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
    checkConfiguration();
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
    checkConfiguration();
    Q_EMIT currentIncomingSecurityProtocolChanged();
}

void ManualConfiguration::setCurrentIncomingProtocol(int newCurrentIncomingProtocol)
{
    if (mCurrentIncomingProtocol != newCurrentIncomingProtocol) {
        mCurrentIncomingProtocol = newCurrentIncomingProtocol;
        if (newCurrentIncomingProtocol == 0) {
            setIncomingPort(995);
        } else {
            setIncomingPort(993);
        }
        checkConfiguration();
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
        checkConfiguration();
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
        checkConfiguration();
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
        checkConfiguration();
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
        checkConfiguration();
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
        checkConfiguration();
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
        checkConfiguration();
        Q_EMIT outgoingUserNameChanged();
    }
}

QDebug operator<<(QDebug d, const ManualConfiguration &t)
{
    d << "mIncomingUserName " << t.incomingUserName();
    d << "mIncomingHostName " << t.incomingHostName();
    d << "mIncomingPort " << t.incomingPort();

    d << "mOutgoingUserName " << t.outgoingUserName();
    d << "mOutgoingHostName " << t.outgoingHostName();
    d << "mOutgoingPort " << t.outgoingPort();

    d << "mCurrentIncomingProtocol " << t.currentIncomingProtocol();
    d << "mCurrentIncomingSecurityProtocol " << t.currentIncomingSecurityProtocol();
    d << "mCurrentOutgoingSecurityProtocol " << t.currentOutgoingSecurityProtocol();

    d << "mCurrentIncomingAuthenticationProtocols " << t.currentIncomingAuthenticationProtocols();
    d << "mCurrentOutgoingAuthenticationProtocols " << t.currentOutgoingAuthenticationProtocols();

    return d;
}

#include "moc_manualconfiguration.cpp"
