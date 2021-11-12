/*
    SPDX-FileCopyrightText: 2009 Volker Krause <vkrause@kde.org>
    SPDX-FileCopyrightText: 2010 Tom Albers <toma@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "page.h"
#include <QStandardItemModel>

#include "ui_providerpage.h"
#include <KNSCore/Engine>
struct Provider {
    QString entryId;
    QString entryProviderId;
};
class QSortFilterProxyModel;

class ProviderPage : public Page
{
    Q_OBJECT
public:
    explicit ProviderPage(KAssistantDialog *parent = nullptr);

    void leavePageNext() override;
    void leavePageNextRequested() override;
    void leavePageBackRequested() override;

    QTreeView *treeview() const;

Q_SIGNALS:
    void ghnsNotWanted();

public Q_SLOTS:
    void startFetchingData();

private:
    void fillModel(const KNSCore::EntryInternal::List &);
    void selectionChanged();
    void providerStatusChanged(const KNSCore::EntryInternal &);
    void slotTextChanged(const QString &str);

    void findDesktopAndSetAssistant(const QStringList &list);

    Ui::ProviderPage ui;
    QStandardItemModel *const m_model;
    QStandardItem *m_fetchItem = nullptr;
    KNSCore::Engine *const m_engine;
    QSortFilterProxyModel *mProxy;
    KNSCore::EntryInternal::List m_providerEntries;
    Provider m_wantedProvider;
    bool m_newPageWanted = false;
    bool m_newPageReady = false;
};

