/*
    SPDX-FileCopyrightText: 2009 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "setupmanager.h"
#include <KAssistantDialog>

class Page;
class TypePage;

class Dialog : public KAssistantDialog
{
    Q_OBJECT
public:
    explicit Dialog(QWidget *parent = nullptr);

    void next() override;
    void back() override;

    // give room for certain pages to create objects too.
    SetupManager *setupManager();

public Q_SLOTS:
    Q_SCRIPTABLE QObject *addPage(const QString &uiFile, const QString &title);

    void reject() override;

private:
    void slotNextPage();
#ifndef ACCOUNTWIZARD_NO_GHNS
    void slotGhnsWanted();
    void slotGhnsNotWanted();
#endif
    void slotManualConfigWanted(bool);
    void slotNextOk();
    void slotBackOk();
    void clearDynamicPages();
    void slotOpenHelp();

private:
    KPageWidgetItem *addPage(Page *page, const QString &title);

private:
    SetupManager *const mSetupManager;
    KPageWidgetItem *mLastPage = nullptr;
    KPageWidgetItem *mProviderPage = nullptr;
    KPageWidgetItem *mTypePage = nullptr;
    KPageWidgetItem *mLoadPage = nullptr;
    QList<KPageWidgetItem *> mDynamicPages;
};

