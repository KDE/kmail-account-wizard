// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "ispdb/configurationmodel.h"
#include "accountwizard_debug.h"
#include <KLocalizedString>

Configuration::Configuration(Server _incoming, std::optional<Server> _outgoing, const QString &_shortDisplayName)
    : incoming(_incoming)
    , outgoing(_outgoing)
    , shortDisplayName(_shortDisplayName)
{}

ConfigurationModel::ConfigurationModel(QObject *parent)
    : QAbstractListModel(parent)
{}

int ConfigurationModel::rowCount(const QModelIndex &parent) const
{
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
        return incomingServer.type == Server::Type::IMAP
            ? i18n("Keep your folders and emails synced on your server")
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
        { NameRole, "name" },
        { DescriptionRole, "description" },
        { IncomingTagsRole, "incomingTags" },
        { IncomingHostRole, "incomingHost" },
        { OutgoingTagsRole, "outgoingTags" },
        { OutgoingHostRole, "outgoingHost" },
    };
}

void ConfigurationModel::setEmailProvider(EmailProvider emailProvider)
{
    std::optional<Server> preferredOutgoingServer;

    for (const auto &server : emailProvider.smtpServers) {
        if (!preferredOutgoingServer
            || preferredOutgoingServer->socketType < server.socketType) {
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
