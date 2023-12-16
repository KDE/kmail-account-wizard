/*
    SPDX-FileCopyrightText: 2009 Volker Krause <vkrause@kde.org>
    SPDX-FileCopyrightText: 2023 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "transport.h"
#include <KLocalizedString>
#include <MailTransport/TransportManager>

Transport::Transport(const QString &type, QObject *parent)
    : SetupBase(parent)
{
    // TODO use it
    Q_UNUSED(type)
}

Transport::~Transport() = default;

void Transport::createTransport()
{
    Q_EMIT info(i18n("Setting up mail transport account..."));
    MailTransport::Transport *mt = MailTransport::TransportManager::self()->createTransport();
    mt->setName(mTransportInfo.name);
    mt->setHost(mTransportInfo.host);
    if (mTransportInfo.port > 0) {
        mt->setPort(mTransportInfo.port);
    }
    if (!mTransportInfo.user.isEmpty()) {
        mt->setUserName(mTransportInfo.user);
        mt->setRequiresAuthentication(true);
    }
    if (!mTransportInfo.password.isEmpty()) {
        mt->setStorePassword(true);
        mt->setPassword(mTransportInfo.password);
    }
    mTransportId = mt->id();
    /*
      // Add convert to int method
        mt->setEncryption(mTransportInfo.encrStr);
        mt->setAuthenticationType(mTransportInfo.authStr);
        */
    mt->save();
    Q_EMIT info(i18n("Mail transport uses '%1' encryption and '%2' authentication.", mTransportInfo.encrStr, mTransportInfo.authStr));
    MailTransport::TransportManager::self()->addTransport(mt);
    MailTransport::TransportManager::self()->setDefaultTransport(mt->id());
    Q_EMIT finished(i18n("Mail transport account set up."));
}

Transport::TransportInfo Transport::transportInfo() const
{
    return mTransportInfo;
}

void Transport::setTransportInfo(const TransportInfo &newTransportInfo)
{
    mTransportInfo = newTransportInfo;
}

int Transport::transportId() const
{
    return mTransportId;
}

QDebug operator<<(QDebug d, const Transport::TransportInfo &t)
{
    d << "name " << t.name;
    d << "host " << t.host;
    d << "user " << t.user;
    d << "password " << t.password;
    d << "encrStr " << t.encrStr;
    d << "authStr " << t.authStr;
    d << "port " << t.port;

    return d;
}

#include "moc_transport.cpp"
