/*
    SPDX-FileCopyrightText: 2023 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "manualconfiguration.h"
#include "accountwizard_debug.h"
#include "servertest.h"
#include <KLocalizedString>
#include <QRegularExpression>
#include <QUrl>

ManualConfiguration::ManualConfiguration(QObject *parent)
    : QObject{parent}
{
}

ManualConfiguration::~ManualConfiguration() = default;

void ManualConfiguration::setEmail(const QString &email)
{
    static QRegularExpression reg(QStringLiteral(".*@"));
    QString hostname = email;
    hostname.remove(reg);
    setIncomingHostName(hostname);
    setOutgoingHostName(hostname);
    setIncomingUserName(email);
    setOutgoingUserName(email);
}

QStringList ManualConfiguration::incomingProtocols() const
{
    return {i18n("POP3"), i18n("IMAP"), i18n("Kolab")};
}

QStringList ManualConfiguration::securityProtocols() const
{
    return {i18n("STARTTLS"), i18n("SSL/TLS"), i18n("None")};
}

QStringList ManualConfiguration::authenticationProtocols() const
{
    return {i18n("Clear text"), i18n("LOGIN"), i18n("PLAIN"), i18n("CRAM-MD5"), i18n("DIGEST-MD5"), i18n("NTLM"), i18n("GSSAPI")};
}

QString ManualConfiguration::generateUniqueAccountName() const
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

Resource::ResourceInfo ManualConfiguration::createPop3Resource() const
{
    Resource::ResourceInfo info;
    info.typeIdentifier = QStringLiteral("akonadi_pop3_resource");
    info.name = generateUniqueAccountName();
    QMap<QString, QVariant> settings;
    settings.insert(QStringLiteral("Port"), mIncomingPort);
    settings.insert(QStringLiteral("Host"), mIncomingHostName);
    settings.insert(QStringLiteral("Login"), mIncomingUserName);
    settings.insert(QStringLiteral("Password"), mPassword);
    // TODO pop3Res.setOption( "UseTLS", true );

    info.settings = settings;
    return info;
}

QString ManualConfiguration::convertIncomingAuthenticationProtocol(int index) const
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

QString ManualConfiguration::convertIncomingSecurityProtocol(int index) const
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

Resource::ResourceInfo ManualConfiguration::createImapResource() const
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

    info.settings = settings;
    return info;
}

Resource::ResourceInfo ManualConfiguration::createKolabResource() const
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
#if 0
    if (servertest_mode < 3) {   // submission & smtp
        if (arg == "tls" ) { // tls is really STARTTLS
          smtp.setEncryption("TLS");
          if (servertest_mode == 1) {   //submission port 587
              smtp.setPort(587);
          } else {
              smtp.setPort(25);
          }
        } else if ( arg == "ssl" ) {    //only possible as smtps
            smtp.setPort(465);
            smtp.setEncryption("SSL");
        } else if (servertest_mode == 2) { //test submission and smtp failed or only possible unencrypted -> set to standard value and open editor
            smtp.setPort(587);
            smtp.setEncryption("TLS");
            smtp.setEditMode(true);
        } else if (servertest_mode == 1) { // submission test failed -> start smtp request
            servertest_mode = 2;
            ServerTest.test(page2.widget().lineEditSmtp.text, "smtp");
            return;
        }

        // start imap test
        servertest_mode = 3;
        if (page2.widget().lineEditImap.text) {
            SetupManager.setupInfo(qsTr("Probing IMAP server..."));
            ServerTest.test(page2.widget().lineEditImap.text, "imap");
        } else {
            SetupManager.execute();
        }
    } else if (servertest_mode == 3) {   //imap
        if ( arg == "ssl" ) {
          // The ENUM used for authentication (in the kolab resource only)
          kolabRes.setOption( "Safety", "SSL" ); // SSL/TLS
          kolabRes.setOption( "ImapPort", 993 );
        } else if ( arg == "tls" ) { // tls is really STARTTLS
          kolabRes.setOption( "Safety", "STARTTLS" );  // STARTTLS
          kolabRes.setOption( "ImapPort", 143 );
        } else {
          // safe default fallback in case server test failed
          kolabRes.setOption( "Safety", "STARTTLS" );
          kolabRes.setOption( "ImapPort", 143 );
          kolabRes.setEditMode(true);
        }
        SetupManager.execute();
    } else {
        print ("Unknown servertest_mode = ", servertest_mode);
    }
#endif
    return info;
}

void ManualConfiguration::createResource()
{
    // Create incoming account
    qDebug() << " createManualAccount ";
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

    auto resource = new Resource(this);
    resource->setResourceInfo(std::move(info));

    connect(resource, &Resource::info, this, &ManualConfiguration::info);
    connect(resource, &Resource::finished, this, &ManualConfiguration::finished);
    connect(resource, &Resource::error, this, &ManualConfiguration::error);
    resource->createResource();
}

void ManualConfiguration::createTransport()
{
    // Create outgoing account
    auto transport = new Transport(this);
    transport->setTransportInfo(std::move(createTransportInfo()));

    connect(transport, &Transport::info, this, &ManualConfiguration::info);
    connect(transport, &Transport::finished, this, &ManualConfiguration::finished);
    connect(transport, &Transport::error, this, &ManualConfiguration::error);
    transport->createTransport();
}

void ManualConfiguration::setPassword(const QString &newPassword)
{
    mPassword = newPassword;
}

void ManualConfiguration::setIdentityId(int id)
{
    mIdentityId = id;
}

void ManualConfiguration::createManualAccount()
{
    createResource();
    createTransport();
}

Transport::TransportInfo ManualConfiguration::createTransportInfo() const
{
    Transport::TransportInfo info;
    info.user = mOutgoingUserName;
    info.host = mOutgoingHostName;
    info.port = mOutgoingPort;
    info.authStr = convertOutgoingAuthenticationProtocol(mCurrentOutgoingAuthenticationProtocol);
    info.encrStr = convertOutgoingSecurityProtocol(mCurrentOutgoingSecurityProtocol);
    return info;
}

QString ManualConfiguration::convertOutgoingSecurityProtocol(int protocol) const
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

QString ManualConfiguration::convertOutgoingAuthenticationProtocol(int protocol) const
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

void ManualConfiguration::checkServer()
{
    qDebug() << " Verify server";
    if (!mServerTest) {
        mServerTest = new ServerTest(this);
        connect(mServerTest, &ServerTest::testFail, this, &ManualConfiguration::slotTestFail);
        connect(mServerTest, &ServerTest::testResult, this, &ManualConfiguration::slotTestResult);
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

void ManualConfiguration::slotTestFail()
{
    qDebug() << "slotTestFail  ";
    // TODO
    mServerTestInProgress = false;
    Q_EMIT serverTestInProgressModeChanged();
}

void ManualConfiguration::slotTestResult(const QString &result)
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

int ManualConfiguration::identityId() const
{
    return mIdentityId;
}

void ManualConfiguration::checkConfiguration()
{
    const bool valid = !mIncomingUserName.trimmed().isEmpty() && !mIncomingHostName.trimmed().isEmpty() && !mOutgoingHostName.trimmed().isEmpty()
        && !mOutgoingUserName.trimmed().isEmpty();
    mConfigurationIsValid = valid;
    Q_EMIT configurationIsValidChanged();
}

int ManualConfiguration::currentOutgoingAuthenticationProtocol() const
{
    return mCurrentOutgoingAuthenticationProtocol;
}

void ManualConfiguration::setCurrentOutgoingAuthenticationProtocol(int newCurrentOutgoingAuthenticationProtocols)
{
    if (mCurrentOutgoingAuthenticationProtocol == newCurrentOutgoingAuthenticationProtocols)
        return;
    mCurrentOutgoingAuthenticationProtocol = newCurrentOutgoingAuthenticationProtocols;
    checkConfiguration();
    Q_EMIT currentOutgoingAuthenticationProtocolChanged();
}

bool ManualConfiguration::disconnectedModeEnabled() const
{
    return mDisconnectedModeEnabled;
}

void ManualConfiguration::setDisconnectedModeEnabled(bool disconnectedMode)
{
    if (mDisconnectedModeEnabled == disconnectedMode)
        return;
    mDisconnectedModeEnabled = disconnectedMode;
    checkConfiguration();
    Q_EMIT disconnectedModeEnabledChanged();
}

int ManualConfiguration::currentIncomingAuthenticationProtocol() const
{
    return mCurrentIncomingAuthenticationProtocol;
}

void ManualConfiguration::setCurrentIncomingAuthenticationProtocol(int newCurrentIncomingAuthenticationProtocols)
{
    if (mCurrentIncomingAuthenticationProtocol == newCurrentIncomingAuthenticationProtocols)
        return;
    mCurrentIncomingAuthenticationProtocol = newCurrentIncomingAuthenticationProtocols;
    checkConfiguration();
    Q_EMIT currentIncomingAuthenticationProtocolChanged();
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

void ManualConfiguration::setCurrentIncomingProtocol(int newCurrentIncomingProtocol)
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

uint ManualConfiguration::incomingPort() const
{
    return mIncomingPort;
}

void ManualConfiguration::setIncomingPort(uint newPort)
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

#include "moc_manualconfiguration.cpp"
