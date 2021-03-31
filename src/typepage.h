/*
    SPDX-FileCopyrightText: 2009 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "page.h"
#include <QStandardItemModel>

#include "ui_typepage.h"
class QSortFilterProxyModel;
class TypePage : public Page
{
    Q_OBJECT
public:
    explicit TypePage(KAssistantDialog *parent = nullptr);

    void leavePageNext() override;
    QTreeView *treeview() const;

Q_SIGNALS:
    void ghnsWanted();

private:
    void selectionChanged();
    void slotTextChanged(const QString &text);

    Ui::TypePage ui;
    QStandardItemModel *m_model = nullptr;
    QSortFilterProxyModel *mProxy = nullptr;
};

