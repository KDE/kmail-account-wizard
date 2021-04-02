// SPDX-FileCopyrightText: 2014 Sandro Knauß <knauss@kolabsys.com>
// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "ispdbhelper.h"
#include "configfile.h"
#include "identity.h"
#include "ispdb/ispdb.h"
#include "ldap.h"
#include "resource.h"
#include "transport.h"

#include <KLocalizedString>

IspdbHelper::IspdbHelper(QObject *parent)
    : IspdbHelper(parent, new Ispdb(this))
{
}

IspdbHelper::IspdbHelper(QObject *parent, Ispdb *ispdb)
    : SetupObject(parent)
    , mIspdb(ispdb)
{
    connect(mIspdb, &Ispdb::finished, this, &IspdbHelper::onIspdbFinished);
    connect(mIspdb, &Ispdb::passwordChanged, this, &IspdbHelper::passwordChanged);
    connect(mIspdb, &Ispdb::emailChanged, this, &IspdbHelper::emailChanged);
}

QStringList IspdbHelper::relevantDomains() const
{
    return mIspdb->relevantDomains();
}

QString IspdbHelper::longName() const
{
    return mIspdb->name(Ispdb::length::Long);
}

QString IspdbHelper::shortName() const
{
    return mIspdb->name(Ispdb::length::Short);
}

int IspdbHelper::defaultIdentity() const
{
    return mIspdb->defaultIdentity();
}

int IspdbHelper::countIdentities() const
{
    return mIspdb->identities().count();
}

void IspdbHelper::fillIdentity(int i, Identity *id) const
{
    // TODO change struct name
    identity isp = mIspdb->identities().at(i);

    id->setIdentityName(isp.name);
    id->setRealName(isp.name);
    id->setEmail(isp.email);
    id->setOrganization(isp.organization);
    id->setSignature(isp.signature);
}

void IspdbHelper::fillImapServer(int i, Resource *imapRes) const
{
    Q_ASSERT(mIspdb->imapServers().count() > i);
    Server isp = mIspdb->imapServers().at(i);

    imapRes->setName(isp.hostname);
    imapRes->setOption(QStringLiteral("ImapServer"), isp.hostname);
    imapRes->setOption(QStringLiteral("UserName"), isp.username);
    imapRes->setOption(QStringLiteral("ImapPort"), isp.port);
    imapRes->setOption(QStringLiteral("Authentication"), isp.authentication); // TODO: setup with right authentication
    if (isp.socketType == Ispdb::None) {
        imapRes->setOption(QStringLiteral("Safety"), QStringLiteral("NONE"));
    } else if (isp.socketType == Ispdb::SSL) {
        imapRes->setOption(QStringLiteral("Safety"), QStringLiteral("SSL"));
    } else {
        imapRes->setOption(QStringLiteral("Safety"), QStringLiteral("STARTTLS"));
    }
}

int IspdbHelper::countImapServers() const
{
    return mIspdb->imapServers().count();
}

void IspdbHelper::fillSmtpServer(int i, Transport *smtpRes) const
{
    Q_ASSERT(mIspdb->smtpServers().count() > i);
    Server isp = mIspdb->smtpServers().at(i);

    smtpRes->setName(isp.hostname);
    smtpRes->setHost(isp.hostname);
    smtpRes->setPort(isp.port);
    smtpRes->setUsername(isp.username);

    switch (isp.authentication) {
    case Ispdb::Plain:
        smtpRes->setAuthenticationType(QStringLiteral("plain"));
        break;
    case Ispdb::CramMD5:
        smtpRes->setAuthenticationType(QStringLiteral("cram-md5"));
        break;
    case Ispdb::NTLM:
        smtpRes->setAuthenticationType(QStringLiteral("ntlm"));
        break;
    case Ispdb::GSSAPI:
        smtpRes->setAuthenticationType(QStringLiteral("gssapi"));
        break;
    case Ispdb::ClientIP:
    case Ispdb::NoAuth:
    default:
        break;
    }
    switch (isp.socketType) {
    case Ispdb::None:
        smtpRes->setEncryption(QStringLiteral("none"));
        break;
    case Ispdb::SSL:
        smtpRes->setEncryption(QStringLiteral("ssl"));
        break;
    case Ispdb::StartTLS:
        smtpRes->setEncryption(QStringLiteral("tls"));
        break;
    }
}

int IspdbHelper::countSmtpServers() const
{
    return mIspdb->smtpServers().count();
}

void IspdbHelper::start()
{
    mIspdb->start();
    Q_EMIT info(i18n("Searching for autoconfiguration..."));
}

void IspdbHelper::setEmail(const QString &email)
{
    mIspdb->setEmail(email);
}

void IspdbHelper::setPassword(const QString &password)
{
    mIspdb->setPassword(password);
}

QString IspdbHelper::email() const
{
    return mIspdb->email();
}

QString IspdbHelper::password() const
{
    return mIspdb->password();
}

void IspdbHelper::create()
{
}

void IspdbHelper::destroy()
{
}

void IspdbHelper::onIspdbFinished(bool status)
{
    Q_EMIT ispdbFinished(status);
    if (status) {
        Q_EMIT info(i18n("Autoconfiguration found."));
    } else {
        Q_EMIT info(i18n("Autoconfiguration failed."));
    }
}
