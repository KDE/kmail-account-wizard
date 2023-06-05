/*
    SPDX-FileCopyrightText: 2009 Volker Krause <vkrause@kde.org>
    SPDX-FileCopyrightText: 2023 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "transport.h"
#include <KLocalizedString>
#include <MailTransport/TransportManager>

Transport::Transport(const QString &type, QObject *parent)
    : QObject(parent)
{
}

Transport::~Transport() = default;

void Transport::createTransport()
{
    // TODO
}

Transport::TransportInfo Transport::transportInfo() const
{
    return mTransportInfo;
}

void Transport::setTransportInfo(const TransportInfo &newTransportInfo)
{
    mTransportInfo = newTransportInfo;
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
