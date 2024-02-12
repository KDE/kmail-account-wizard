// SPDX-FileCopyrightText: 2024 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

#pragma once

#include <QAbstractListModel>
#include <QQmlEngine>

class ConsoleLog : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

public:
    enum EntryType { Success, Info, Warning, Error };
    Q_ENUM(EntryType)

    enum ExtraRoles {
        OutputRole = Qt::UserRole + 1,
        EntryTypeRole,
    };

    explicit ConsoleLog(QObject *parent = nullptr);

    QVariant data(const QModelIndex &index, int role) const override;
    int rowCount(const QModelIndex &parent = {}) const override;
    QHash<int, QByteArray> roleNames() const override;

    inline void success(const QString &output)
    {
        addEntry(output, EntryType::Success);
    }

    inline void error(const QString &output)
    {
        addEntry(output, EntryType::Error);
    }

    inline void info(const QString &output)
    {
        addEntry(output, EntryType::Info);
    }

    inline void warning(const QString &output)
    {
        addEntry(output, EntryType::Warning);
    }

    void addEntry(const QString &output, EntryType type);

private:
    struct LogEntry {
        QString output;
        ConsoleLog::EntryType type;
    };

    std::vector<LogEntry> m_logEntries;
};
