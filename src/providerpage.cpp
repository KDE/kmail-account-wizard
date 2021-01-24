/*
    SPDX-FileCopyrightText: 2009 Volker Krause <vkrause@kde.org>
    SPDX-FileCopyrightText: 2010 Tom Albers <toma@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "providerpage.h"
#include "global.h"

#include "accountwizard_debug.h"
#include <QSortFilterProxyModel>
#include <KLocalizedString>
#include <QLineEdit>

ProviderPage::ProviderPage(KAssistantDialog *parent)
    : Page(parent)
    , m_model(new QStandardItemModel(this))
    , m_engine(new KNSCore::Engine(this))
{
    ui.setupUi(this);

    const QString name = QCoreApplication::applicationName() + QStringLiteral(".knsrc");
    m_engine->init(name);
    mProxy = new QSortFilterProxyModel(this);
    mProxy->setSourceModel(m_model);
    mProxy->setFilterKeyColumn(-1);
    mProxy->setFilterCaseSensitivity(Qt::CaseInsensitive);
    ui.listView->setModel(mProxy);
    connect(ui.searchLine, &QLineEdit::textChanged, this, &ProviderPage::slotTextChanged);
    m_fetchItem = new QStandardItem(i18n("Fetching provider list..."));
    m_model->appendRow(m_fetchItem);
    m_fetchItem->setFlags(Qt::NoItemFlags);

    // we can start the search, whenever the user reaches this page, chances
    // are we have the full list.
    connect(m_engine, &KNSCore::Engine::signalUpdateableEntriesLoaded, this, &ProviderPage::fillModel);
    connect(m_engine, &KNSCore::Engine::signalEntryEvent, this, [this](const KNSCore::EntryInternal &entry, KNSCore::EntryInternal::EntryEvent event) {
        if (event == KNSCore::EntryInternal::StatusChangedEvent) {
            providerStatusChanged(entry);
        }
    });
    m_engine->setSortMode(KNSCore::Provider::Alphabetical);

    connect(ui.listView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &ProviderPage::selectionChanged);
}

void ProviderPage::startFetchingData()
{
    m_engine->requestData(0, 100000);
}

void ProviderPage::slotTextChanged(const QString &str)
{
    mProxy->setFilterFixedString(str);
}

void ProviderPage::fillModel(const KNSCore::EntryInternal::List &list)
{
    if (m_fetchItem) {
        m_model->removeRows(m_model->indexFromItem(m_fetchItem).row(), 1);
        m_fetchItem = nullptr;
    }

    // KNS3::Entry::Entry() is private, so we need to save the whole list.
    // we can not use a QHash or whatever, as that needs that constructor...
    m_providerEntries = list;

    for (const KNSCore::EntryInternal &e : list) {
        qCDebug(ACCOUNTWIZARD_LOG) << "Found Entry: " << e.name();

        auto *item = new QStandardItem(e.name());
        item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        item->setData(e.name(), Qt::ToolTipRole);
        item->setData(e.uniqueId(), Qt::UserRole);
        item->setData(e.providerId(), Qt::UserRole + 1);
        m_model->appendRow(item);
    }
}

void ProviderPage::selectionChanged()
{
    if (ui.listView->selectionModel()->hasSelection()) {
        setValid(true);
    } else {
        setValid(false);
    }
}

void ProviderPage::leavePageNext()
{
    m_newPageReady = false;
    if (!ui.listView->selectionModel()->hasSelection()) {
        return;
    }
    const QModelIndex index = ui.listView->selectionModel()->selectedIndexes().at(0);
    if (!index.isValid()) {
        return;
    }

    const auto *proxy = static_cast<const QSortFilterProxyModel *>(ui.listView->model());
    const QStandardItem *item = m_model->itemFromIndex(proxy->mapToSource(index));
    qCDebug(ACCOUNTWIZARD_LOG) << "Item selected:" << item->text();

    // download and execute it...
    for (const KNSCore::EntryInternal &e : qAsConst(m_providerEntries)) {
        if (e.uniqueId() == item->data(Qt::UserRole)
            && e.providerId() == item->data(Qt::UserRole + 1)) {
            m_wantedProvider.entryId = e.uniqueId();
            m_wantedProvider.entryProviderId = e.providerId();

            if (e.status() == KNS3::Entry::Installed) {
                qCDebug(ACCOUNTWIZARD_LOG) << "already installed" << e.installedFiles();
                findDesktopAndSetAssistant(e.installedFiles());
            } else {
                qCDebug(ACCOUNTWIZARD_LOG) << "Starting download for " << e.name();
                m_engine->install(e);
            }

            break;
        }
    }
}

void ProviderPage::providerStatusChanged(const KNSCore::EntryInternal &e)
{
    qCDebug(ACCOUNTWIZARD_LOG) << e.name();
    if (e.uniqueId() == m_wantedProvider.entryId
        && e.providerId() == m_wantedProvider.entryProviderId
        && e.status() == KNS3::Entry::Installed) {
        findDesktopAndSetAssistant(e.installedFiles());
    }
}

void ProviderPage::findDesktopAndSetAssistant(const QStringList &list)
{
    for (const QString &file : list) {
        qCDebug(ACCOUNTWIZARD_LOG) << file;
        if (file.endsWith(QLatin1String(".desktop"))) {
            qCDebug(ACCOUNTWIZARD_LOG) << "Yay, a desktop file!" << file;
            Global::setAssistant(file);
            m_newPageReady = true;
            if (m_newPageWanted) {
                qCDebug(ACCOUNTWIZARD_LOG) << "New page was already requested, now we are done, approve it";
                Q_EMIT leavePageNextOk();
            }
            break;
        }
    }
}

QTreeView *ProviderPage::treeview() const
{
    return ui.listView;
}

void ProviderPage::leavePageBackRequested()
{
    Q_EMIT leavePageBackOk();
    Q_EMIT ghnsNotWanted();
}

void ProviderPage::leavePageNextRequested()
{
    m_newPageWanted = true;
    if (m_newPageReady) {
        qCDebug(ACCOUNTWIZARD_LOG) << "New page requested and we are done, so ok...";
        Q_EMIT leavePageNextOk();
    } else {
        qCDebug(ACCOUNTWIZARD_LOG) << "New page requested, but we are not done yet...";
    }
}
