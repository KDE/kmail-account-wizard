// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

#pragma once

#include "serverconfiguration.h"
#include <QAbstractListModel>
#include <QtQmlIntegration/qqmlintegration.h>
#include <optional>

class ConsoleLog;

struct Configuration {
    explicit Configuration(const Server &incomming, std::optional<Server> outgoing, const QString &shortDisplayName, std::optional<GroupwareServer> groupware);
    QString shortDisplayName;
    Server incoming;
    std::optional<Server> outgoing;
    std::optional<GroupwareServer> groupware;
};

/// Model exposing the various configuration found by fetching the ispdb
/// in an user friendly way.
class ConfigurationModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(bool hasGroupwareSupport READ hasGroupwareSupport NOTIFY hasGroupwareSupportChanged)

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

    bool hasGroupwareSupport() const;

    void setEmailProvider(const EmailProvider &emailProvider);
    void setPassword(const QString &password);
    void setFullName(const QString &fullName);
    void setEmail(const QString &email);
    void clear();

    const Configuration &configuration(int index) const;

    Q_INVOKABLE void createAutomaticAccount(int index, ConsoleLog *consoleLog, bool groupware);

Q_SIGNALS:
    void hasGroupwareSupportChanged();

private:
    QString m_password;
    QString m_fullName;
    QString m_email;
    bool m_hasGroupwareSupport = false;
    std::vector<Configuration> m_configurations;
};
