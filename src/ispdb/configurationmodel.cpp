// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "ispdb/configurationmodel.h"
#include "accountconfiguration.h"
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

void ConfigurationModel::createAutomaticAccount(int index, ConsoleLog *consoleLog)
{
    qCDebug(ACCOUNTWIZARD_LOG) << " Create Automatic Account" << index;
    const auto &configuration = m_configurations[index];
    qCDebug(ACCOUNTWIZARD_LOG) << configuration.incoming;

    auto accountConfiguration = new AccountConfiguration(KIdentityManagementCore::IdentityManager::self(), this);
    accountConfiguration->setEmail(m_email);
    if (!m_fullName.isEmpty()) {
        accountConfiguration->identity().setFullName(m_fullName);
    } else {
        auto unnamed = i18nc("Default name for new email accounts/identities.", "Unnamed");
        accountConfiguration->identity().setFullName(unnamed);
    }
    accountConfiguration->identity().setPrimaryEmailAddress(m_email);

    // Create resource
    accountConfiguration->setIncomingProtocol(configuration.incoming.type == Server::Type::POP3 ? AccountConfiguration::POP3 : AccountConfiguration::IMAP);
    accountConfiguration->setIncomingHostName(configuration.incoming.hostname);
    accountConfiguration->setIncomingPort(configuration.incoming.port);
    accountConfiguration->setPassword(m_password);

    accountConfiguration->setIncomingAuthenticationProtocol(configuration.incoming.authType);
    accountConfiguration->setIncomingSecurityProtocol(configuration.incoming.socketType);

    // Create SMTP transport
    if (configuration.outgoing) {
        auto mt = accountConfiguration->mailTransport();
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

        mt->setAuthenticationType(configuration.outgoing->authType);
        mt->setEncryption(configuration.outgoing->socketType);
    } else {
        accountConfiguration->setHasTransport(false);
    }
    accountConfiguration->save(consoleLog);
}
