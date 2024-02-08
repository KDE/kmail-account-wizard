/*
    SPDX-FileCopyrightText: 2023-2024 Laurent Montel <montel.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "accountconfigurationbase.h"
#include "accountwizard_debug.h"
#include "servertest.h"
#include <KLocalizedString>
#include <QRegularExpression>
#include <QUrl>

AccountConfigurationBase::AccountConfigurationBase(QObject *parent)
    : QObject{parent}
{
}

AccountConfigurationBase::~AccountConfigurationBase() = default;

void AccountConfigurationBase::setEmail(const QString &email)
{
    static QRegularExpression reg(QStringLiteral(".*@"));
    QString hostname = email;
    hostname.remove(reg);
    setIncomingHostName(hostname);
    setOutgoingHostName(hostname);
    setIncomingUserName(email);
    setOutgoingUserName(email);
}

QStringList AccountConfigurationBase::incomingProtocols() const
{
    return {i18n("POP3"), i18n("IMAP"), i18n("Kolab")};
}

QStringList AccountConfigurationBase::securityProtocols() const
{
    return {i18n("STARTTLS"), i18n("SSL/TLS"), i18n("None")};
}

QStringList AccountConfigurationBase::authenticationProtocols() const
{
    return {i18n("Clear text"), i18n("LOGIN"), i18n("PLAIN"), i18n("CRAM-MD5"), i18n("DIGEST-MD5"), i18n("NTLM"), i18n("GSSAPI")};
}

QString AccountConfigurationBase::generateUniqueAccountName() const
{
    QString name;
    switch (mCurrentIncomingProtocol) {
    case 0: // Pop3
        name = QStringLiteral("Pop3 (%1)").arg(mIncomingHostName);
        break;
    case 1: // Imap
        name = QStringLiteral("Imap (%1)").arg(mIncomingHostName);
        break;
    case 2: // Kolab
        name = QStringLiteral("Kolab (%1)").arg(mIncomingHostName);
        break;
    default:
        qCWarning(ACCOUNTWIZARD_LOG) << " invalid protocol: " << mCurrentIncomingProtocol;
        break;
    }
    return name;
}

Resource::ResourceInfo AccountConfigurationBase::createPop3Resource() const
{
    Resource::ResourceInfo info;
    info.typeIdentifier = QStringLiteral("akonadi_pop3_resource");
    info.name = generateUniqueAccountName();
    QMap<QString, QVariant> settings;
    settings.insert(QStringLiteral("Port"), mIncomingPort);
    settings.insert(QStringLiteral("Host"), mIncomingHostName);
    settings.insert(QStringLiteral("Login"), mIncomingUserName);
    settings.insert(QStringLiteral("Password"), mPassword);
    settings.insert(QStringLiteral("UseTLS"), true);

    info.settings = settings;
    return info;
}

QString AccountConfigurationBase::convertIncomingAuthenticationProtocol(int protocol) const
{
    switch (protocol) {
    case 0: // Clear Text
        return QStringLiteral("clear");
    case 1: // LOGIN
        return QStringLiteral("login");
    case 2: // PLAIN
        return QStringLiteral("plain");
    case 3: // CRAM-MD5
        return QStringLiteral("cram-md5");
    case 4: // DIGEST-MD5
        return QStringLiteral("digest-md5");
    case 5: // NTLM
        return QStringLiteral("ntlm");
    case 6: // GSSAPI
        return QStringLiteral("gssapi");
    }
    qCWarning(ACCOUNTWIZARD_LOG) << " Impossible to convert protocol: " << protocol;
    return {};
}

QString AccountConfigurationBase::convertIncomingSecurityProtocol(int index) const
{
    switch (index) {
    case 0:
        return QStringLiteral("STARTTLS");
    case 1:
        return QStringLiteral("SSL");
    case 2:
        return QStringLiteral("NONE");
    }
    Q_UNREACHABLE();
    return {};
}

Resource::ResourceInfo AccountConfigurationBase::createImapResource() const
{
    Resource::ResourceInfo info;
    info.typeIdentifier = QStringLiteral("akonadi_imap_resource");
    info.name = generateUniqueAccountName();
    QMap<QString, QVariant> settings;
    settings.insert(QStringLiteral("ImapServer"), mIncomingPort);
    settings.insert(QStringLiteral("UserName"), mIncomingUserName);
    settings.insert(QStringLiteral("DisconnectedModeEnabled"), mDisconnectedModeEnabled);
    settings.insert(QStringLiteral("Password"), mPassword);
    settings.insert(QStringLiteral("ImapPort"), mIncomingPort);
    settings.insert(QStringLiteral("IntervalCheckTime"), 60);
    settings.insert(QStringLiteral("SubscriptionEnabled"), true);
    settings.insert(QStringLiteral("UseDefaultIdentity"), false);
    settings.insert(QStringLiteral("AccountIdentity"), mIdentityId);
    settings.insert(QStringLiteral("Authentication"), convertIncomingAuthenticationProtocol(mCurrentIncomingAuthenticationProtocol));
    settings.insert(QStringLiteral("Safety"), convertIncomingSecurityProtocol(mCurrentIncomingSecurityProtocol));
    info.settings = settings;
    return info;
}

Resource::ResourceInfo AccountConfigurationBase::createKolabResource() const
{
    Resource::ResourceInfo info;
    info.name = generateUniqueAccountName();
    QMap<QString, QVariant> settings;
    settings.insert(QStringLiteral("ImapServer"), mIncomingPort);
    settings.insert(QStringLiteral("UserName"), mIncomingUserName);
    settings.insert(QStringLiteral("DisconnectedModeEnabled"), true);
    settings.insert(QStringLiteral("AccountIdentity"), mIdentityId);
    settings.insert(QStringLiteral("UseDefaultIdentity"), false);
    settings.insert(QStringLiteral("SieveSupport"), true);
    settings.insert(QStringLiteral("IntervalCheckTime"), 60);
    settings.insert(QStringLiteral("SubscriptionEnabled"), true);
    settings.insert(QStringLiteral("Password"), mPassword);
    settings.insert(QStringLiteral("ImapPort"), mIncomingPort);
    info.settings = settings;
    info.typeIdentifier = QStringLiteral("akonadi_kolab_resource");
    return info;
}

void AccountConfigurationBase::createResource()
{
    // Create incoming account
    qCDebug(ACCOUNTWIZARD_LOG) << " createManualAccount ";
    Resource::ResourceInfo info;
    switch (mCurrentIncomingProtocol) {
    case 0: // Pop3
        info = createPop3Resource();
        break;
    case 1: // Imap
        info = createImapResource();
        break;
    case 2: // Kolab
        info = createKolabResource();
        break;
    default:
        qCWarning(ACCOUNTWIZARD_LOG) << " invalid protocol: " << mCurrentIncomingProtocol;
        return;
    }
    // Debug it
    qCDebug(ACCOUNTWIZARD_LOG) << " info " << info;

    generateResource(std::move(info));
}

void AccountConfigurationBase::generateResource(const Resource::ResourceInfo &info)
{
    qDebug() << " info " << info;
    // Reimplement
}

void AccountConfigurationBase::createTransport()
{
    // TODO reimplement
}

void AccountConfigurationBase::setPassword(const QString &newPassword)
{
    mPassword = newPassword;
}

void AccountConfigurationBase::setIdentityId(int id)
{
    mIdentityId = id;
}

void AccountConfigurationBase::createManualAccount()
{
    createResource();
    createTransport();
}

Transport::TransportInfo AccountConfigurationBase::createTransportInfo() const
{
    Transport::TransportInfo info;
    info.user = mOutgoingUserName;
    info.host = mOutgoingHostName;
    info.port = mOutgoingPort;
    info.authStr = convertOutgoingAuthenticationProtocol(mCurrentOutgoingAuthenticationProtocol);
    info.encrStr = convertOutgoingSecurityProtocol(mCurrentOutgoingSecurityProtocol);
    return info;
}

QString AccountConfigurationBase::convertOutgoingSecurityProtocol(int protocol) const
{
    switch (protocol) {
    case 0:
        return QStringLiteral("TLS");
    case 1:
        return QStringLiteral("SSL");
    case 2:
        return QStringLiteral("None");
    }
    Q_UNREACHABLE();
    return {};
}

QString AccountConfigurationBase::convertOutgoingAuthenticationProtocol(int protocol) const
{
    switch (protocol) {
    case 0: // Clear Text
        return QStringLiteral("clear");
    case 1: // LOGIN
        return QStringLiteral("login");
    case 2: // PLAIN
        return QStringLiteral("plain");
    case 3: // CRAM-MD5
        return QStringLiteral("cram-md5");
    case 4: // DIGEST-MD5
        return QStringLiteral("digest-md5");
    case 5: // NTLM
        return QStringLiteral("ntlm");
    case 6: // GSSAPI
        return QStringLiteral("gssapi");
    }
    qCWarning(ACCOUNTWIZARD_LOG) << " Impossible to convert protocol: " << protocol;
    return {};
}

void AccountConfigurationBase::checkServer()
{
    qDebug() << " Verify server";
    if (!mServerTest) {
        mServerTest = new ServerTest(this);
        connect(mServerTest, &ServerTest::testFail, this, &AccountConfigurationBase::slotTestFail);
        connect(mServerTest, &ServerTest::testResult, this, &AccountConfigurationBase::slotTestResult);
    }
    QString protocol;
    switch (mCurrentIncomingProtocol) {
    case 0: // Pop3
        protocol = QStringLiteral("pop");
        break;
    case 1: // Imap
        protocol = QStringLiteral("imap");
        break;
    case 2: // Kolab
        protocol = QStringLiteral("imap");
        break;
    }
    // Test input
    mServerTestInProgress = true;
    Q_EMIT serverTestInProgressModeChanged();
    mServerTest->test(mIncomingHostName, protocol);
}

void AccountConfigurationBase::slotTestFail()
{
    qDebug() << "slotTestFail  ";
    // TODO
    mServerTestInProgress = false;
    Q_EMIT serverTestInProgressModeChanged();
}

void AccountConfigurationBase::slotTestResult(const QString &result)
{
    qDebug() << "slotTestResult  " << result;
    switch (mCurrentIncomingProtocol) {
    case 0: { // Pop3
        if (result == QStringLiteral("ssl")) {
            setIncomingPort(995);
            // pop3Res.setOption( "UseTLS", true );
        } else if (result == QStringLiteral("tls")) { // tls is really STARTTLS
            setIncomingPort(110);
            // pop3Res.setOption( "UseTLS", true );
        } else if (result == QStringLiteral("none")) {
            setIncomingPort(110);
        } else {
            setIncomingPort(110);
        }
        break;
    }
    case 1: { // Imap
        if (result == QStringLiteral("ssl")) {
            setIncomingPort(993);
            // pop3Res.setOption( "UseTLS", true );
        } else if (result == QStringLiteral("tls")) { // tls is really STARTTLS
            setIncomingPort(143);
            // pop3Res.setOption( "UseTLS", true );
        } else if (result == QStringLiteral("none")) {
            setIncomingPort(143);
        } else {
            setIncomingPort(143);
        }
        break;
#if 0
        // if ( server == "imap.gmail.com" ) {
        //     imapRes.setOption( "Authentication", 9 ); // XOAuth2
        //     arg = "ssl";
        // } else {
        //     imapRes.setOption( "Authentication", 7 ); // ClearText
        // }
        // if ( arg == "ssl" ) {
        //   // The ENUM used for authentication (in the imap resource only)
        //   imapRes.setOption( "Safety", "SSL"); // SSL/TLS
        //   imapRes.setOption( "ImapPort", 993 );
        // } else if ( arg == "tls" ) { // tls is really STARTTLS
        //   imapRes.setOption( "Safety", "STARTTLS");  // STARTTLS
        //   imapRes.setOption( "ImapPort", 143 );
        // } else if ( arg == "none" ) {
        //   imapRes.setOption( "Safety", "NONE" );  // No encryption
        //   imapRes.setOption( "ImapPort", 143 );
        // } else {
        //   // safe default fallback when servertest failed
        //   imapRes.setOption( "Safety", "STARTTLS");
        //   imapRes.setOption( "ImapPort", 143 );
        // }
#endif
        break;
    }
    }

    // TODO
    mServerTestInProgress = false;
    Q_EMIT serverTestInProgressModeChanged();
}

int AccountConfigurationBase::identityId() const
{
    return mIdentityId;
}

void AccountConfigurationBase::checkConfiguration()
{
    const bool valid = !mIncomingUserName.trimmed().isEmpty() && !mIncomingHostName.trimmed().isEmpty() && !mOutgoingHostName.trimmed().isEmpty()
        && !mOutgoingUserName.trimmed().isEmpty();
    mConfigurationIsValid = valid;
    Q_EMIT configurationIsValidChanged();
}

int AccountConfigurationBase::currentOutgoingAuthenticationProtocol() const
{
    return mCurrentOutgoingAuthenticationProtocol;
}

void AccountConfigurationBase::setCurrentOutgoingAuthenticationProtocol(int newCurrentOutgoingAuthenticationProtocols)
{
    if (mCurrentOutgoingAuthenticationProtocol == newCurrentOutgoingAuthenticationProtocols)
        return;
    mCurrentOutgoingAuthenticationProtocol = newCurrentOutgoingAuthenticationProtocols;
    checkConfiguration();
    Q_EMIT currentOutgoingAuthenticationProtocolChanged();
}

bool AccountConfigurationBase::disconnectedModeEnabled() const
{
    return mDisconnectedModeEnabled;
}

void AccountConfigurationBase::setDisconnectedModeEnabled(bool disconnectedMode)
{
    if (mDisconnectedModeEnabled == disconnectedMode)
        return;
    mDisconnectedModeEnabled = disconnectedMode;
    checkConfiguration();
    Q_EMIT disconnectedModeEnabledChanged();
}

int AccountConfigurationBase::currentIncomingAuthenticationProtocol() const
{
    return mCurrentIncomingAuthenticationProtocol;
}

void AccountConfigurationBase::setCurrentIncomingAuthenticationProtocol(int newCurrentIncomingAuthenticationProtocols)
{
    if (mCurrentIncomingAuthenticationProtocol == newCurrentIncomingAuthenticationProtocols)
        return;
    mCurrentIncomingAuthenticationProtocol = newCurrentIncomingAuthenticationProtocols;
    checkConfiguration();
    Q_EMIT currentIncomingAuthenticationProtocolChanged();
}

int AccountConfigurationBase::currentOutgoingSecurityProtocol() const
{
    return mCurrentOutgoingSecurityProtocol;
}

void AccountConfigurationBase::setCurrentOutgoingSecurityProtocol(int newCurrentOutgoingSecurityProtocol)
{
    if (mCurrentOutgoingSecurityProtocol == newCurrentOutgoingSecurityProtocol)
        return;
    mCurrentOutgoingSecurityProtocol = newCurrentOutgoingSecurityProtocol;
    checkConfiguration();
    Q_EMIT currentOutgoingSecurityProtocolChanged();
}

int AccountConfigurationBase::currentIncomingSecurityProtocol() const
{
    return mCurrentIncomingSecurityProtocol;
}

void AccountConfigurationBase::setCurrentIncomingSecurityProtocol(int newCurrentIncomingSecurityProtocol)
{
    if (mCurrentIncomingSecurityProtocol == newCurrentIncomingSecurityProtocol)
        return;
    mCurrentIncomingSecurityProtocol = newCurrentIncomingSecurityProtocol;
    if (mCurrentIncomingProtocol == 0) { // Pop3
        if (mCurrentIncomingSecurityProtocol == 0) { // StartTLS
            setIncomingPort(110);
        } else if (mCurrentIncomingSecurityProtocol == 1) { // SSL
            setIncomingPort(995);
        } else {
            setIncomingPort(110);
        }
    } else if (mCurrentIncomingProtocol == 1) { // Imap
        if (mCurrentIncomingSecurityProtocol == 0) { // StartTLS
            setIncomingPort(143);
        } else if (mCurrentIncomingSecurityProtocol == 1) { // SSL
            setIncomingPort(993);
        } else {
            setIncomingPort(143);
        }
    } else {
        // FIXME ?
        setIncomingPort(993);
    }
    checkConfiguration();
    Q_EMIT currentIncomingSecurityProtocolChanged();
}

void AccountConfigurationBase::setCurrentIncomingProtocol(int newCurrentIncomingProtocol)
{
    if (mCurrentIncomingProtocol != newCurrentIncomingProtocol) {
        mCurrentIncomingProtocol = newCurrentIncomingProtocol;
        if (newCurrentIncomingProtocol == 0) { // Pop3
            setIncomingPort(995);
            mHasDisconnectedMode = false;
        } else {
            setIncomingPort(993);
            mHasDisconnectedMode = true;
        }
        checkConfiguration();
        Q_EMIT currentIncomingProtocolChanged();
        Q_EMIT hasDisconnectedModeChanged();
    }
}

int AccountConfigurationBase::currentIncomingProtocol() const
{
    return mCurrentIncomingProtocol;
}

QString AccountConfigurationBase::incomingHostName() const
{
    return mIncomingHostName;
}

void AccountConfigurationBase::setIncomingHostName(const QString &newIncomingHostName)
{
    if (mIncomingHostName != newIncomingHostName) {
        mIncomingHostName = newIncomingHostName;
        checkConfiguration();
        Q_EMIT incomingHostNameChanged();
    }
}

uint AccountConfigurationBase::incomingPort() const
{
    return mIncomingPort;
}

void AccountConfigurationBase::setIncomingPort(uint newPort)
{
    if (mIncomingPort != newPort) {
        mIncomingPort = newPort;
        checkConfiguration();
        Q_EMIT incomingPortChanged();
    }
}

QString AccountConfigurationBase::incomingUserName() const
{
    return mIncomingUserName;
}

void AccountConfigurationBase::setIncomingUserName(const QString &newIncomingUserName)
{
    if (mIncomingUserName != newIncomingUserName) {
        mIncomingUserName = newIncomingUserName;
        checkConfiguration();
        Q_EMIT incomingUserNameChanged();
    }
}

QString AccountConfigurationBase::outgoingHostName() const
{
    return mOutgoingHostName;
}

void AccountConfigurationBase::setOutgoingHostName(const QString &newOutgoingHostName)
{
    if (mOutgoingHostName != newOutgoingHostName) {
        mOutgoingHostName = newOutgoingHostName;
        checkConfiguration();
        Q_EMIT outgoingHostNameChanged();
    }
}

int AccountConfigurationBase::outgoingPort() const
{
    return mOutgoingPort;
}

void AccountConfigurationBase::setOutgoingPort(int newPort)
{
    if (mOutgoingPort != newPort) {
        mOutgoingPort = newPort;
        checkConfiguration();
        Q_EMIT outgoingPortChanged();
    }
}

QString AccountConfigurationBase::outgoingUserName() const
{
    return mOutgoingUserName;
}

void AccountConfigurationBase::setOutgoingUserName(const QString &newOutgoingUserName)
{
    if (mOutgoingUserName != newOutgoingUserName) {
        mOutgoingUserName = newOutgoingUserName;
        checkConfiguration();
        Q_EMIT outgoingUserNameChanged();
    }
}

QDebug operator<<(QDebug d, const AccountConfigurationBase &t)
{
    d.space() << "mIncomingUserName" << t.incomingUserName();
    d.space() << "mIncomingHostName" << t.incomingHostName();
    d.space() << "mIncomingPort" << t.incomingPort();

    d.space() << "mOutgoingUserName" << t.outgoingUserName();
    d.space() << "mOutgoingHostName" << t.outgoingHostName();
    d.space() << "mOutgoingPort" << t.outgoingPort();

    d.space() << "mCurrentIncomingProtocol" << t.currentIncomingProtocol();
    d.space() << "mCurrentIncomingSecurityProtocol" << t.currentIncomingSecurityProtocol();
    d.space() << "mCurrentOutgoingSecurityProtocol" << t.currentOutgoingSecurityProtocol();

    d.space() << "mCurrentIncomingAuthenticationProtocol" << t.currentIncomingAuthenticationProtocol();
    d.space() << "mCurrentOutgoingAuthenticationProtocol" << t.currentOutgoingAuthenticationProtocol();

    d.space() << "mDisconnectedModeEnabled" << t.disconnectedModeEnabled();
    d.space() << "identity" << t.identityId();
    return d;
}

#include "moc_accountconfigurationbase.cpp"
