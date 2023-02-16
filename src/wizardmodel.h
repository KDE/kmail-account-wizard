/*
    SPDX-FileCopyrightText: 2020 Carl Schwan <carlschwan@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <KPluginMetaData>
#include <QAbstractListModel>

class WizardModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int assistant READ assistant WRITE setAssistant NOTIFY assistantChanged)
public:
    enum Roles { PluginId = Qt::UserRole };

public:
    explicit WizardModel(QObject *parent = nullptr);
    ~WizardModel();
    Q_REQUIRED_RESULT int rowCount(const QModelIndex &parent) const override;
    Q_REQUIRED_RESULT QVariant data(const QModelIndex &index, int role) const override;
    Q_REQUIRED_RESULT QHash<int, QByteArray> roleNames() const override;

    void setAssistant(int assistant);
    Q_REQUIRED_RESULT int assistant() const;

Q_SIGNALS:
    void assistantChanged();

private:
    void reload();

    QList<KPluginMetaData> m_items;
    int m_assistant = -1;
};
