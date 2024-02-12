/*
    SPDX-FileCopyrightText: 2023-2024 Laurent Montel <montel.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "manualconfiguration.h"
#include "accountwizard_debug.h"
#include "servertest.h"
#include <KIMAP/LoginJob>
#include <KIdentityManagementCore/IdentityManager>
#include <KLocalizedString>
#include <MailTransport/TransportManager>
#include <QRegularExpression>
#include <QUrl>

ManualConfiguration::ManualConfiguration(QObject *parent)
    : QObject{parent}
    , mMailTransport(MailTransport::TransportManager::self()->createTransport())
    , mIdentity(KIdentityManagementCore::IdentityManager::self()->newFromScratch(QString()))
{
    // Set appropriate defaults
    mMailTransport->setStorePassword(true);
    mMailTransport->setEncryption(MailTransport::Transport::EnumEncryption::SSL);
    mMailTransport->setPort(587);
    mMailTransport->setAuthenticationType(MailTransport::Transport::EnumAuthenticationType::PLAIN);
}

ManualConfiguration::~ManualConfiguration() = default;

MailTransport::Transport *ManualConfiguration::mailTransport() const
{
    return mMailTransport;
}

void ManualConfiguration::setIdentity(const KIdentityManagementCore::Identity &identity)
{
    mIdentity = identity;
    Q_EMIT identityChanged();
}

KIdentityManagementCore::Identity ManualConfiguration::identity() const
{
    return mIdentity;
}

QString ManualConfiguration::email() const
{
    return mIdentity.primaryEmailAddress();
}

void ManualConfiguration::setEmail(const QString &email)
{
    mIdentity.setPrimaryEmailAddress(email);

    static QRegularExpression reg(QStringLiteral(".*@"));
    QString hostname = email;
    hostname.remove(reg);
    setIncomingHostName(hostname);
    setIncomingUserName(email);

    mMailTransport->setName(hostname);
    mMailTransport->setHost(hostname);
    mMailTransport->setUserName(email);
}

QString ManualConfiguration::generateUniqueAccountName() const
{
    QString name;
    switch (mIncomingProtocol) {
    case POP3:
        name = QStringLiteral("Pop3 (%1)").arg(mIncomingHostName);
        break;
    case IMAP:
        name = QStringLiteral("Imap (%1)").arg(mIncomingHostName);
        break;
    case KOLAB:
        name = QStringLiteral("Kolab (%1)").arg(mIncomingHostName);
        break;
    default:
        qCWarning(ACCOUNTWIZARD_LOG) << " invalid protocol: " << mIncomingProtocol;
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
    settings.insert(QStringLiteral("AccountIdentity"), mIdentity.uoid());
    settings.insert(QStringLiteral("Authentication"), mIncomingAuthenticationProtocol);
    settings.insert(QStringLiteral("Safety"), mIncomingSecurityProtocol);
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
    settings.insert(QStringLiteral("AccountIdentity"), mIdentity.uoid());
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

void ManualConfiguration::generateResource(const Resource::ResourceInfo &info)
{
    auto resource = new Resource(this);
    resource->setResourceInfo(std::move(info));

    connect(resource, &Resource::info, this, &ManualConfiguration::info);
    connect(resource, &Resource::finished, this, &ManualConfiguration::finished);
    connect(resource, &Resource::error, this, &ManualConfiguration::error);
    resource->createResource();
}

QString ManualConfiguration::password() const
{
    return mPassword;
}

void ManualConfiguration::setPassword(const QString &password)
{
    if (mPassword == password) {
        return;
    }
    mPassword = password;
    Q_EMIT passwordChanged();
}

void ManualConfiguration::save()
{
    // create resource
    Resource::ResourceInfo info;
    switch (mIncomingProtocol) {
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
        qCWarning(ACCOUNTWIZARD_LOG) << " invalid protocol: " << mIncomingProtocol;
        return;
    }
    qCDebug(ACCOUNTWIZARD_LOG) << " info " << info;
    generateResource(std::move(info));

    // create transport
    mMailTransport->setPassword(mPassword);
    MailTransport::TransportManager::self()->addTransport(mMailTransport);
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
    switch (mIncomingProtocol) {
    case POP3:
        protocol = QStringLiteral("pop");
        break;
    case IMAP:
        protocol = QStringLiteral("imap");
        break;
    case KOLAB:
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
    switch (mIncomingProtocol) {
    case IncomingProtocol::POP3: { // Pop3
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
    case IncomingProtocol::IMAP: { // Imap
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

void ManualConfiguration::checkConfiguration()
{
    const bool valid = !mIncomingUserName.trimmed().isEmpty() && !mIncomingHostName.trimmed().isEmpty() && !mMailTransport->host().trimmed().isEmpty()
        && !mMailTransport->userName().trimmed().isEmpty();
    if (mConfigurationIsValid == valid) {
        return;
    }
    mConfigurationIsValid = valid;
    Q_EMIT configurationIsValidChanged();
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

KIMAP::LoginJob::AuthenticationMode ManualConfiguration::incomingAuthenticationProtocol() const
{
    return mIncomingAuthenticationProtocol;
}

void ManualConfiguration::setIncomingAuthenticationProtocol(KIMAP::LoginJob::AuthenticationMode newIncomingAuthenticationProtocols)
{
    if (mIncomingAuthenticationProtocol == newIncomingAuthenticationProtocols) {
        return;
    }

    mIncomingAuthenticationProtocol = newIncomingAuthenticationProtocols;
    checkConfiguration();
    Q_EMIT incomingAuthenticationProtocolChanged();
}

KIMAP::LoginJob::EncryptionMode ManualConfiguration::incomingSecurityProtocol() const
{
    return mIncomingSecurityProtocol;
}

void ManualConfiguration::setIncomingSecurityProtocol(KIMAP::LoginJob::EncryptionMode securityProtocol)
{
    if (mIncomingSecurityProtocol == securityProtocol) {
        return;
    }

    mIncomingSecurityProtocol = securityProtocol;

    if (mIncomingProtocol == POP3) {
        if (mIncomingSecurityProtocol == KIMAP::LoginJob::STARTTLS) {
            setIncomingPort(110);
        } else if (mIncomingSecurityProtocol == KIMAP::LoginJob::SSLorTLS) {
            setIncomingPort(995);
        } else {
            setIncomingPort(110);
        }
    } else if (mIncomingProtocol == IMAP) { // Imap
        if (mIncomingSecurityProtocol == KIMAP::LoginJob::STARTTLS) {
            setIncomingPort(143);
        } else if (mIncomingSecurityProtocol == KIMAP::LoginJob::SSLorTLS) {
            setIncomingPort(993);
        } else {
            setIncomingPort(143);
        }
    } else {
        // FIXME ?
        setIncomingPort(993);
    }
    checkConfiguration();
    Q_EMIT incomingSecurityProtocolChanged();
}

void ManualConfiguration::setIncomingProtocol(IncomingProtocol newIncomingProtocol)
{
    if (mIncomingProtocol != newIncomingProtocol) {
        mIncomingProtocol = newIncomingProtocol;
        if (newIncomingProtocol == 0) { // Pop3
            setIncomingPort(995);
            mHasDisconnectedMode = false;
        } else {
            setIncomingPort(993);
            mHasDisconnectedMode = true;
        }
        checkConfiguration();
        Q_EMIT incomingProtocolChanged();
        Q_EMIT hasDisconnectedModeChanged();
    }
}

ManualConfiguration::IncomingProtocol ManualConfiguration::incomingProtocol() const
{
    return mIncomingProtocol;
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

QDebug operator<<(QDebug d, const ManualConfiguration &t)
{
    d.space() << "mIncomingUserName" << t.incomingUserName();
    d.space() << "mIncomingHostName" << t.incomingHostName();
    d.space() << "mIncomingPort" << t.incomingPort();

    d.space() << "mIncomingProtocol" << t.incomingProtocol();
    d.space() << "mIncomingSecurityProtocol" << t.incomingSecurityProtocol();

    d.space() << "mIncomingAuthenticationProtocol" << t.incomingAuthenticationProtocol();

    d.space() << "mDisconnectedModeEnabled" << t.disconnectedModeEnabled();
    d.space() << "identity" << t.identity().uoid();
    return d;
}

#include "moc_manualconfiguration.cpp"
