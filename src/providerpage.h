/*
    Copyright (c) 2009 Volker Krause <vkrause@kde.org>
    Copyright (c) 2010 Tom Albers <toma@kde.org>

    This library is free software; you can redistribute it and/or modify it
    under the terms of the GNU Library General Public License as published by
    the Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    This library is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
    License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to the
    Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
    02110-1301, USA.
*/

#ifndef PROVIDERPAGE_H
#define PROVIDERPAGE_H

#include "page.h"
#include <QStandardItemModel>

#include "ui_providerpage.h"
#include <KNSCore/DownloadManager>
struct Provider {
    QString entryId;
    QString entryProviderId;
};
class QSortFilterProxyModel;
namespace KNSCore {
class DownloadManager;
}

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
    QStandardItemModel *m_model = nullptr;
    QStandardItem *m_fetchItem = nullptr;
    KNSCore::DownloadManager *m_downloadManager = nullptr;
    QSortFilterProxyModel *mProxy;
    KNSCore::EntryInternal::List m_providerEntries;
    Provider m_wantedProvider;
    bool m_newPageWanted;
    bool m_newPageReady;
};

#endif
