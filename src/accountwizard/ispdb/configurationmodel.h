// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

#pragma once

#include "serverconfiguration.h"
#include <QAbstractListModel>
#include <optional>

struct Configuration {
    explicit Configuration(const Server &incomming, std::optional<Server> outgoing, const QString &shortDisplayName);
    QString shortDisplayName;
    Server incoming;
    std::optional<Server> outgoing;
};

/// Model exposing the various configuration found by fetching the ispdb
/// in an user friendly way.
class ConfigurationModel : public QAbstractListModel
{
public:
    explicit ConfigurationModel(QObject *parent = nullptr);

    enum ExtraRoles {
        NameRole = Qt::UserRole + 1,
        DescriptionRole,
        IncomingTagsRole,
        IncomingHostRole,
        OutgoingTagsRole,
        OutgoingHostRole,
    };

    int rowCount(const QModelIndex &parent = {}) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    void setEmailProvider(const EmailProvider &emailProvider);
    void clear();

private:
    std::vector<Configuration> m_configurations;
};
