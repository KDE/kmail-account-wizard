/*
    SPDX-FileCopyrightText: 2023-2024 Laurent Montel <montel.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "manualconfiguration.h"
#include "accountwizard_debug.h"
#include "consolelog.h"
#include "servertest.h"
#include <KLocalizedString>
#include <MailTransport/TransportManager>
#include <QQmlEngine>
#include <QRegularExpression>
#include <QUrl>

using namespace Qt::Literals::StringLiterals;

ManualConfiguration::ManualConfiguration(KIdentityManagementCore::IdentityManager *manager, QObject *parent)
    : QObject{parent}
    , mMailTransport(MailTransport::TransportManager::self()->createTransport())
    , mIdentityManager(manager)
    , mIdentity(mIdentityManager->newFromScratch(QStringLiteral("tmp")))
{
    // Set appropriate defaults
    mMailTransport->setStorePassword(true);
    mMailTransport->setEncryption(MailTransport::Transport::EnumEncryption::SSL);
    mMailTransport->setPort(587);
    mMailTransport->setAuthenticationType(MailTransport::Transport::EnumAuthenticationType::PLAIN);
}

ManualConfiguration::ManualConfiguration(QObject *parent)
    : QObject{parent}
    , mMailTransport(MailTransport::TransportManager::self()->createTransport())
    , mIdentityManager(KIdentityManagementCore::IdentityManager::self())
    , mIdentity(mIdentityManager->newFromScratch(QStringLiteral("tmp2")))
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

KIdentityManagementCore::Identity &ManualConfiguration::identity() const
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

    static QRegularExpression reg(u".*@"_s);
    QString hostname = email;
    hostname.remove(reg);
    setIncomingHostName(hostname);
    setIncomingUserName(email);

    mMailTransport->setName(u"SMTP (%1)"_s.arg(email));
    mMailTransport->setHost(hostname);
    mMailTransport->setUserName(email);
}

QString ManualConfiguration::generateUniqueAccountName() const
{
    QString name;
    switch (mIncomingProtocol) {
    case POP3:
        name = u"Pop3 (%1)"_s.arg(mIncomingHostName);
        break;
    case IMAP:
        name = u"Imap (%1)"_s.arg(mIncomingHostName);
        break;
    case KOLAB:
        name = u"Kolab (%1)"_s.arg(mIncomingHostName);
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
    info.typeIdentifier = u"akonadi_pop3_resource"_s;
    info.name = generateUniqueAccountName();
    QMap<QString, QVariant> settings;
    settings.insert(u"Port"_s, mIncomingPort);
    settings.insert(u"Host"_s, mIncomingHostName);
    settings.insert(u"Login"_s, mIncomingUserName);
    settings.insert(u"Password"_s, mPassword);
    settings.insert(u"UseTLS"_s, true);

    info.settings = settings;
    return info;
}

Resource::ResourceInfo ManualConfiguration::createImapResource() const
{
    Resource::ResourceInfo info;
    info.typeIdentifier = u"akonadi_imap_resource"_s;
    info.name = generateUniqueAccountName();
    QMap<QString, QVariant> settings;
    settings.insert(u"ImapServer"_s, mIncomingHostName);
    settings.insert(u"UserName"_s, mIncomingUserName);
    settings.insert(u"DisconnectedModeEnabled"_s, mDisconnectedModeEnabled);
    settings.insert(u"Password"_s, mPassword);
    settings.insert(u"ImapPort"_s, mIncomingPort);
    settings.insert(u"IntervalCheckTime"_s, 60);
    settings.insert(u"SubscriptionEnabled"_s, true);
    settings.insert(u"UseDefaultIdentity"_s, false);
    settings.insert(u"AccountIdentity"_s, mIdentity.uoid());
    settings.insert(u"Authentication"_s, mIncomingAuthenticationProtocol);
    settings.insert(u"Safety"_s, mIncomingSecurityProtocol);
    info.settings = settings;
    return info;
}

Resource::ResourceInfo ManualConfiguration::createKolabResource() const
{
    Resource::ResourceInfo info;
    info.name = generateUniqueAccountName();
    QMap<QString, QVariant> settings;
    settings.insert(u"ImapServer"_s, mIncomingHostName);
    settings.insert(u"UserName"_s, mIncomingUserName);
    settings.insert(u"DisconnectedModeEnabled"_s, true);
    settings.insert(u"AccountIdentity"_s, mIdentity.uoid());
    settings.insert(u"UseDefaultIdentity"_s, false);
    settings.insert(u"SieveSupport"_s, true);
    settings.insert(u"IntervalCheckTime"_s, 60);
    settings.insert(u"SubscriptionEnabled"_s, true);
    settings.insert(u"Password"_s, mPassword);
    settings.insert(u"ImapPort"_s, mIncomingPort);
    info.settings = settings;
    info.typeIdentifier = u"akonadi_kolab_resource"_s;
    return info;
}

void ManualConfiguration::generateResource(const Resource::ResourceInfo &info, ConsoleLog *consoleLog)
{
    auto resource = new Resource(consoleLog, this);
    resource->setResourceInfo(std::move(info));
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

void ManualConfiguration::save(ConsoleLog *consoleLog)
{
    // create identity
    QString identityName;
    QString hostName;
    if (email().split(u'@').count() > 1) {
        hostName = email().split(u'@')[1];
    } else {
        hostName = mIncomingHostName;
    }
    auto unnamed = i18nc("Default name for new email accounts/identities.", "Unnamed");
    if (!mIdentity.fullName().isEmpty()) {
        identityName = u"%1 (%2)"_s.arg(mIdentity.fullName(), hostName);
    } else {
        identityName = u"%1 (%2)"_s.arg(unnamed, hostName);
    }
    if (!mIdentityManager->isUnique(identityName)) {
        identityName = mIdentityManager->makeUnique(identityName);
    }
    mIdentity.setIdentityName(identityName);

    QString identityLogEntryText = u"<h3>"_s + i18nc("log entry content", "Create email identity: %1", identityName) + u"</h3>"_s;

    identityLogEntryText += u"<ul>"_s;
    identityLogEntryText += u"<li><b>%1</b> %2</li>"_s.arg(i18nc("log entry content", "Full name:"), mIdentity.fullName());
    identityLogEntryText += u"<li><b>%1</b> %2</li>"_s.arg(i18nc("log entry content", "Primary email address:"), email());
    identityLogEntryText += u"</ul>"_s;
    consoleLog->success(identityLogEntryText);

    // create resource
    Resource::ResourceInfo info;
    switch (mIncomingProtocol) {
    case IncomingProtocol::POP3:
        info = createPop3Resource();
        break;
    case IncomingProtocol::IMAP:
        info = createImapResource();
        break;
    case IncomingProtocol::KOLAB:
        info = createKolabResource();
        break;
    default:
        qCWarning(ACCOUNTWIZARD_LOG) << " invalid protocol: " << mIncomingProtocol;
        return;
    }
    qCDebug(ACCOUNTWIZARD_LOG) << " info " << info;
    generateResource(std::move(info), consoleLog);

    // create transport
    using TransportAuth = MailTransport::Transport::EnumAuthenticationType;
    mMailTransport->setPassword(mPassword);
    MailTransport::TransportManager::self()->addTransport(mMailTransport);

    qWarning() << mMailTransport->port();

    QString logEntryText = u"<h3>"_s + i18nc("log entry content", "Mail transport setup completed: %1", mMailTransport->name()) + u"</h3>"_s;

    logEntryText += u"<ul>"_s;
    logEntryText += u"<li><b>%1</b> %2</li>"_s.arg(i18nc("log entry content", "Host:"), mMailTransport->host());
    logEntryText += u"<li><b>%1</b> %2</li>"_s.arg(i18nc("log entry content", "Port:"), QString::number(mMailTransport->port()));
    logEntryText += u"<li><b>%1</b> %2</li>"_s.arg(i18nc("log entry content", "Username:"), mMailTransport->userName());
    logEntryText +=
        u"<li><b>%1</b> %2</li>"_s.arg(i18nc("log entry content", "Encryption:"),
                                       QLatin1String(QMetaEnum::fromType<MailTransport::Transport::EnumEncryption>().key(mMailTransport->encryption())));
    logEntryText += u"<li><b>%1</b> %2</li>"_s.arg(i18nc("log entry content", "Authentication:"),
                                                   QLatin1String(QMetaEnum::fromType<TransportAuth>().key(mMailTransport->authenticationType())));
    logEntryText += u"</ul>"_s;
    consoleLog->success(logEntryText);

    mIdentity.setTransport(QString::number(mMailTransport->id()));
    mIdentityManager->saveIdentity(mIdentity);
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
        protocol = u"pop"_s;
        break;
    case IMAP:
        protocol = u"imap"_s;
        break;
    case KOLAB:
        protocol = u"imap"_s;
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
        if (result == u"ssl"_s) {
            setIncomingPort(995);
            // pop3Res.setOption( "UseTLS", true );
        } else if (result == u"tls"_s) { // tls is really STARTTLS
            setIncomingPort(110);
            // pop3Res.setOption( "UseTLS", true );
        } else if (result == u"none"_s) {
            setIncomingPort(110);
        } else {
            setIncomingPort(110);
        }
        break;
    }
    case IncomingProtocol::IMAP: { // Imap
        if (result == u"ssl"_s) {
            setIncomingPort(993);
            // pop3Res.setOption( "UseTLS", true );
        } else if (result == u"tls"_s) { // tls is really STARTTLS
            setIncomingPort(143);
            // pop3Res.setOption( "UseTLS", true );
        } else if (result == u"none"_s) {
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

MailTransport::Transport::EnumAuthenticationType ManualConfiguration::incomingAuthenticationProtocol() const
{
    return mIncomingAuthenticationProtocol;
}

void ManualConfiguration::setIncomingAuthenticationProtocol(MailTransport::Transport::EnumAuthenticationType newIncomingAuthenticationProtocols)
{
    if (mIncomingAuthenticationProtocol == newIncomingAuthenticationProtocols) {
        return;
    }

    mIncomingAuthenticationProtocol = newIncomingAuthenticationProtocols;
    checkConfiguration();
    Q_EMIT incomingAuthenticationProtocolChanged();
}

MailTransport::Transport::EnumEncryption ManualConfiguration::incomingSecurityProtocol() const
{
    return mIncomingSecurityProtocol;
}

void ManualConfiguration::setIncomingSecurityProtocol(MailTransport::Transport::EnumEncryption securityProtocol)
{
    if (mIncomingSecurityProtocol == securityProtocol) {
        return;
    }

    mIncomingSecurityProtocol = securityProtocol;

    if (mIncomingProtocol == POP3) {
        if (mIncomingSecurityProtocol == MailTransport::Transport::EnumEncryption::TLS) {
            setIncomingPort(110);
        } else if (mIncomingSecurityProtocol == MailTransport::Transport::EnumEncryption::SSL) {
            setIncomingPort(995);
        } else {
            setIncomingPort(110);
        }
    } else if (mIncomingProtocol == IMAP) {
        if (mIncomingSecurityProtocol == MailTransport::Transport::EnumEncryption::TLS) {
            setIncomingPort(143);
        } else if (mIncomingSecurityProtocol == MailTransport::Transport::EnumEncryption::SSL) {
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
