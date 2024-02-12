// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "ispdb/configurationmodel.h"
#include "accountwizard_debug.h"
#include "consolelog.h"
#include "resource.h"
#include <KIdentityManagementCore/Identity>
#include <KIdentityManagementCore/IdentityManager>
#include <KLocalizedString>
#include <MailTransport/TransportManager>
#include <QMetaEnum>
#include <QQmlEngine>

using namespace Qt::Literals::StringLiterals;

Configuration::Configuration(const Server &_incoming, std::optional<Server> _outgoing, const QString &_shortDisplayName)
    : shortDisplayName(_shortDisplayName)
    , incoming(_incoming)
    , outgoing(_outgoing)
{
}

ConfigurationModel::ConfigurationModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int ConfigurationModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_configurations.size();
}

QVariant ConfigurationModel::data(const QModelIndex &index, int role) const
{
    Q_ASSERT(checkIndex(index, QAbstractItemModel::CheckIndexOption::IndexIsValid));

    const auto &configuration = m_configurations[index.row()];
    const auto incomingServer = configuration.incoming;
    const auto outgoingServer = configuration.outgoing;

    switch (role) {
    case Qt::DisplayRole:
    case NameRole:
        return incomingServer.type == Server::Type::IMAP ? i18n("IMAP") : i18n("POP3");
    case DescriptionRole:
        return incomingServer.type == Server::Type::IMAP ? i18n("Keep your folders and emails synced on your server")
                                                         : i18n("Keep your folders and emails on your computer");
    case IncomingHostRole:
        return incomingServer.hostname;
    case IncomingTagsRole:
        return incomingServer.tags();
    case OutgoingHostRole:
        return outgoingServer ? outgoingServer->hostname : QString();
    case OutgoingTagsRole:
        return outgoingServer ? outgoingServer->tags() : QStringList();
    }
    return {};
}

QHash<int, QByteArray> ConfigurationModel::roleNames() const
{
    return {
        {NameRole, "name"},
        {DescriptionRole, "description"},
        {IncomingTagsRole, "incomingTags"},
        {IncomingHostRole, "incomingHost"},
        {OutgoingTagsRole, "outgoingTags"},
        {OutgoingHostRole, "outgoingHost"},
    };
}

void ConfigurationModel::clear()
{
    beginResetModel();
    m_configurations.clear();
    endResetModel();
}

void ConfigurationModel::setEmailProvider(const EmailProvider &emailProvider)
{
    std::optional<Server> preferredOutgoingServer;

    for (const auto &server : emailProvider.smtpServers) {
        if (!preferredOutgoingServer || preferredOutgoingServer->socketType < server.socketType) {
            preferredOutgoingServer = server;
        }
    }

    beginResetModel();
    m_configurations.clear();

    for (const auto &server : emailProvider.imapServers) {
        m_configurations.emplace_back(server, preferredOutgoingServer, emailProvider.shortDisplayName);
    }

    for (const auto &server : emailProvider.popServers) {
        m_configurations.emplace_back(server, preferredOutgoingServer, emailProvider.shortDisplayName);
    }

    endResetModel();
}

void ConfigurationModel::setPassword(const QString &password)
{
    m_password = password;
}

void ConfigurationModel::setEmail(const QString &email)
{
    m_email = email;
}

void ConfigurationModel::setFullName(const QString &fullName)
{
    m_fullName = fullName;
}

const Configuration &ConfigurationModel::configuration(int index) const
{
    return m_configurations[index];
}

void ConfigurationModel::createAutomaticAccount(int index)
{
    auto engine = qmlEngine(parent());
    Q_ASSERT(engine);
    auto consoleLog = engine->singletonInstance<ConsoleLog *>(u"org.kde.pim.accountwizard"_s, u"ConsoleLog"_s);
    Q_ASSERT(consoleLog);

    qCDebug(ACCOUNTWIZARD_LOG) << " Create Automatic Account" << index;
    const auto &configuration = m_configurations[index];
    qCDebug(ACCOUNTWIZARD_LOG) << configuration.incoming;

    // Create identity
    auto identityManager = KIdentityManagementCore::IdentityManager::self();
    auto identity = identityManager->newFromScratch(QString());

    QString identityName;
    QString fullName;
    QString hostName;
    if (m_email.split(u'@').count() > 1) {
        hostName = m_email.split(u'@')[1];
    } else {
        hostName = configuration.incoming.hostname;
    }
    auto unnamed = i18nc("Default name for new email accounts/identities.", "Unnamed");
    if (!m_fullName.isEmpty() && !configuration.incoming.hostname.isEmpty()) {
        identityName = u"%1 (%2)"_s.arg(m_fullName, hostName);
        fullName = m_fullName;
    } else if (!configuration.incoming.hostname.isEmpty()) {
        identityName = u"%1 (%2)"_s.arg(unnamed, hostName);
        fullName = unnamed;
    } else {
        identityName = m_fullName;
        fullName = m_fullName;
    }
    if (!identityManager->isUnique(identityName)) {
        identityName = identityManager->makeUnique(identityName);
    }
    identity.setIdentityName(identityName);
    identity.setFullName(fullName);
    identity.setPrimaryEmailAddress(m_email);
    identityManager->saveIdentity(identity);
    identityManager->commit();

    QString identityLogEntryText = u"<h2>"_s + i18nc("log entry content", "Create email identity: %1", identityName) + u"</h2>"_s;

    identityLogEntryText += u"<ul>"_s;
    identityLogEntryText += u"<li><b>%1</b> %2</li>"_s.arg(i18nc("log entry content", "Full name:"), fullName);
    identityLogEntryText += u"<li><b>%1</b> %2</li>"_s.arg(i18nc("log entry content", "Primary email address:"), m_email);
    identityLogEntryText += u"</ul>"_s;
    consoleLog->success(identityLogEntryText);

    // Create resource
    Resource::ResourceInfo info;
    QMap<QString, QVariant> settings;
    switch (configuration.incoming.type) {
    case Server::Type::POP3:
        info.typeIdentifier = u"akonadi_pop_resource"_s;
        info.name = u"Pop3 (%1)"_s.arg(configuration.incoming.username);
        settings.insert(u"Port"_s, configuration.incoming.port);
        settings.insert(u"Host"_s, configuration.incoming.hostname);
        settings.insert(u"Login"_s, configuration.incoming.username);
        settings.insert(u"Password"_s, m_password);
        settings.insert(u"AccountIdentity"_s, identity.uoid());
        break;
    case Server::Type::IMAP: {
        info.typeIdentifier = u"akonadi_imap_resource"_s;
        info.name = u"IMAP (%1)"_s.arg(configuration.incoming.username);
        settings.insert(u"ImapServer"_s, configuration.incoming.hostname);
        settings.insert(u"UserName"_s, configuration.incoming.username);
        settings.insert(u"DisconnectedModeEnabled"_s, true);
        settings.insert(u"Password"_s, m_password);
        settings.insert(u"ImapPort"_s, configuration.incoming.port);
        settings.insert(u"IntervalCheckTime"_s, 60);
        settings.insert(u"SubscriptionEnabled"_s, true);
        settings.insert(u"UseDefaultIdentity"_s, false);
        settings.insert(u"AccountIdentity"_s, identity.uoid());
        qWarning() << "AccountIdentity" << identity.uoid();

        using TransportAuth = MailTransport::Transport::EnumAuthenticationType;
        qWarning() << configuration.incoming.authType;
        switch (configuration.incoming.authType) {
        case AuthType::Plain:
            settings.insert(u"Authentication"_s, TransportAuth::PLAIN);
            break;
        case AuthType::CramMD5:
            settings.insert(u"Authentication"_s, TransportAuth::CRAM_MD5);
            break;
        case AuthType::NTLM:
            settings.insert(u"Authentication"_s, TransportAuth::NTLM);
            break;
        case AuthType::GSSAPI:
            settings.insert(u"Authentication"_s, TransportAuth::GSSAPI);
            break;
        case AuthType::ClientIP:
        case AuthType::NoAuth:
            settings.insert(u"Authentication"_s, TransportAuth::ANONYMOUS);
            break;
        case AuthType::OAuth2:
            settings.insert(u"Authentication"_s, TransportAuth::XOAUTH2);
            break;
        }

        using TransportEncryption = MailTransport::Transport::EnumEncryption;
        switch (configuration.incoming.socketType) {
        case SSL:
            settings.insert(u"Safety"_s, TransportEncryption::SSL);
            break;
        case StartTLS:
            settings.insert(u"Safety"_s, TransportEncryption::TLS);
            break;
        case None:
            settings.insert(u"Safety"_s, TransportEncryption::None);
            break;
        }

        break;
    }
    case Server::Type::SMTP:
        Q_UNREACHABLE();
    }

    info.settings = settings;

    auto resource = new Resource(consoleLog, this); // will be deleted later once finished
    resource->setResourceInfo(std::move(info));
    resource->createResource();

    // Create SMTP transport
    if (configuration.outgoing) {
        qWarning() << "create transport";
        MailTransport::Transport *mt = MailTransport::TransportManager::self()->createTransport();
        mt->setName(u"SMTP (%1)"_s.arg(configuration.incoming.username));
        mt->setHost(configuration.outgoing->hostname);
        if (configuration.outgoing->port > 0) {
            mt->setPort(configuration.outgoing->port);
        }

        if (!configuration.outgoing->username.isEmpty()) {
            mt->setUserName(configuration.outgoing->username);
            mt->setRequiresAuthentication(true);
        }

        if (!m_password.isEmpty()) {
            mt->setStorePassword(true);
            mt->setPassword(m_password);
        }

        using TransportAuth = MailTransport::Transport::EnumAuthenticationType;

        QHash<AuthType, TransportAuth> mappingAuthentificationType = {
            {Plain, TransportAuth::PLAIN},
            {CramMD5, TransportAuth::CRAM_MD5},
            {NTLM, TransportAuth::NTLM},
            {GSSAPI, TransportAuth::GSSAPI},
            {ClientIP, TransportAuth::ANONYMOUS},
            {NoAuth, TransportAuth::ANONYMOUS},
            {OAuth2, TransportAuth::XOAUTH2},
        };

        mt->setAuthenticationType(mappingAuthentificationType[configuration.outgoing->authType]);

        QHash<SocketType, MailTransport::Transport::EnumEncryption> mappingEncryption = {
            {SSL, MailTransport::Transport::EnumEncryption::SSL},
            {StartTLS, MailTransport::Transport::EnumEncryption::TLS},
            {None, MailTransport::Transport::EnumEncryption::None},
        };

        mt->setEncryption(mappingEncryption[configuration.outgoing->socketType]);

        QString logEntryText = u"<h3>"_s + i18nc("log entry content", "Mail transport setup completed: %1", mt->name()) + u"</h3>"_s;

        logEntryText += u"<ul>"_s;
        logEntryText += u"<li><b>%1</b> %2</li>"_s.arg(i18nc("log entry content", "Host:"), mt->host());
        logEntryText += u"<li><b>%1</b> %2</li>"_s.arg(i18nc("log entry content", "Port:"), mt->port());
        logEntryText += u"<li><b>%1</b> %2</li>"_s.arg(i18nc("log entry content", "Username:"), mt->userName());
        logEntryText += u"<li><b>%1</b> %2</li>"_s.arg(i18nc("log entry content", "Encryption:"),
                                                       QLatin1String(QMetaEnum::fromType<MailTransport::Transport::EnumEncryption>().key(mt->encryption())));
        logEntryText += u"<li><b>%1</b> %2</li>"_s.arg(i18nc("log entry content", "Authentication:"),
                                                       QLatin1String(QMetaEnum::fromType<TransportAuth>().key(mt->authenticationType())));
        logEntryText += u"</ul>"_s;
        consoleLog->success(logEntryText);

        identity.setTransport(QString::number(mt->id()));
        identityManager->saveIdentity(identity);
        identityManager->commit();

        MailTransport::TransportManager::self()->addTransport(mt);
    }
}
