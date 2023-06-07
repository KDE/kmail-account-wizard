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
}

Transport::~Transport() = default;

void Transport::createTransport()
{
    Q_EMIT info(i18n("Setting up mail transport account..."));
    MailTransport::Transport *mt = MailTransport::TransportManager::self()->createTransport();
    /*
        mt->setName(m_name);
        mt->setHost(m_host);
        if (m_port > 0) {
            mt->setPort(m_port);
        }
        if (!m_user.isEmpty()) {
            mt->setUserName(m_user);
            mt->setRequiresAuthentication(true);
        }
        if (!m_password.isEmpty()) {
            mt->setStorePassword(true);
            mt->setPassword(m_password);
        }
        mt->setEncryption(m_encr);
        mt->setAuthenticationType(m_auth);
        m_transportId = mt->id();
        */
    mt->save();
    // TODO Q_EMIT info(i18n("Mail transport uses '%1' encryption and '%2' authentication.", m_encrStr, m_authStr));
    MailTransport::TransportManager::self()->addTransport(mt);
    MailTransport::TransportManager::self()->setDefaultTransport(mt->id());
    Q_EMIT finished(i18n("Mail transport account set up."));

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
