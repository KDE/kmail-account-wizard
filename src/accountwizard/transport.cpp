/*
    SPDX-FileCopyrightText: 2009 Volker Krause <vkrause@kde.org>
    SPDX-FileCopyrightText: 2023 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "transport.h"
#include "accountwizard_debug.h"
#include <KLocalizedString>
#include <MailTransport/Transport>
#include <MailTransport/TransportManager>

#define TABLE_SIZE x

template<typename T>
struct StringValueTable {
    const char *name;
    using value_type = T;
    value_type value;
};

static const StringValueTable<MailTransport::Transport::EnumEncryption> encryptionEnum[] = {{"none", MailTransport::Transport::EnumEncryption::None},
                                                                                            {"ssl", MailTransport::Transport::EnumEncryption::SSL},
                                                                                            {"tls", MailTransport::Transport::EnumEncryption::TLS}};
static const int encryptionEnumSize = sizeof(encryptionEnum) / sizeof(*encryptionEnum);

static const StringValueTable<MailTransport::Transport::EnumAuthenticationType> authenticationTypeEnum[] = {
    {"login", MailTransport::Transport::EnumAuthenticationType::LOGIN},
    {"plain", MailTransport::Transport::EnumAuthenticationType::PLAIN},
    {"cram-md5", MailTransport::Transport::EnumAuthenticationType::CRAM_MD5},
    {"digest-md5", MailTransport::Transport::EnumAuthenticationType::DIGEST_MD5},
    {"gssapi", MailTransport::Transport::EnumAuthenticationType::GSSAPI},
    {"ntlm", MailTransport::Transport::EnumAuthenticationType::NTLM},
    {"apop", MailTransport::Transport::EnumAuthenticationType::APOP},
    {"clear", MailTransport::Transport::EnumAuthenticationType::CLEAR},
    {"oauth2", MailTransport::Transport::EnumAuthenticationType::XOAUTH2},
    {"anonymous", MailTransport::Transport::EnumAuthenticationType::ANONYMOUS}};
static const int authenticationTypeEnumSize = sizeof(authenticationTypeEnum) / sizeof(*authenticationTypeEnum);

template<typename T>
static typename T::value_type stringToValue(const T *table, const int tableSize, const QString &string, bool &valid)
{
    const QString ref = string.toLower();
    for (int i = 0; i < tableSize; ++i) {
        if (ref == QLatin1String(table[i].name)) {
            valid = true;
            return table[i].value;
        }
    }
    valid = false;
    return table[0].value; // TODO: error handling
}

Transport::Transport(QObject *parent)
    : SetupBase(parent)
{
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

    bool valid;
    QString encrStr;
    MailTransport::Transport::EnumEncryption encr = stringToValue(encryptionEnum, encryptionEnumSize, mTransportInfo.encrStr, valid);
    if (valid) {
        qCWarning(ACCOUNTWIZARD_LOG) << "encr " << encr;
        encrStr = mTransportInfo.encrStr;
    }
    mt->setEncryption(encr);

    QString authStr;
    MailTransport::Transport::EnumAuthenticationType auth = stringToValue(authenticationTypeEnum, authenticationTypeEnumSize, mTransportInfo.authStr, valid);
    if (valid) {
        qCWarning(ACCOUNTWIZARD_LOG) << "auth " << auth;
        authStr = mTransportInfo.authStr;
    }
    mt->setAuthenticationType(auth);

    mt->save();
    Q_EMIT info(i18n("Mail transport uses '%1' encryption and '%2' authentication.", encrStr, authStr));
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
