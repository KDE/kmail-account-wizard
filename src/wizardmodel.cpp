/*
    SPDX-FileCopyrightText: 2020 Carl Schwan <carlschwan@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "wizardmodel.h"

#include <KConfigGroup>
#include <KDesktopFile>

#include <KPackage/PackageLoader>
#include <QDir>
#include <QMimeDatabase>
#include <QMimeType>
#include <QStandardPaths>
#include <kpackage/package.h>

WizardModel::WizardModel(QObject *parent)
    : QAbstractListModel(parent)
{
    reload();
}

void WizardModel::reload()
{
    beginResetModel();
    m_items.clear();
    endResetModel();

    const auto list = KPackage::PackageLoader::self()->listPackages(QStringLiteral("AccountWizard/Wizard"));
    // NOTE: This will disable completely the internal in-memory cache
    KPackage::Package p;
    p.install(QString(), QString());

    m_items = list;
}

WizardModel::~WizardModel() = default;

QVariant WizardModel::data(const QModelIndex &index, int role) const
{
    if (index.row() >= m_items.count()) {
        return {};
    }

    const auto &item = m_items[index.row()];
    switch (role) {
    case Qt::ToolTipRole:
        return item.description();
    case Qt::DisplayRole:
        return item.name();
    case Qt::DecorationRole:
        return item.iconName();
    case PluginId:
        return item.pluginId();
    default:
        return {};
    }
}

int WizardModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_items.count();
}

void WizardModel::setAssistant(int assistant)
{
    if (m_assistant == assistant) {
        return;
    }
    m_assistant = assistant;
    Q_EMIT assistantChanged();
}

int WizardModel::assistant() const
{
    return m_assistant;
}

QHash<int, QByteArray> WizardModel::roleNames() const
{
    auto roles = QAbstractListModel::roleNames();
    roles[PluginId] = QByteArrayLiteral("pluginId");
    return roles;
}
