/*
    SPDX-FileCopyrightText: 2009 Volker Krause <vkrause@kde.org>
    SPDX-FileCopyrightText: 2010 Tom Albers <toma@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dialog.h"
#include "personaldatapage.h"
#ifndef ACCOUNTWIZARD_NO_GHNS
#include "providerpage.h"
#endif
#include "cryptopage.h"
#include "dynamicpage.h"
#include "global.h"
#include "loadpage.h"
#include "servertest.h"
#include "setuppage.h"
#include "typepage.h"

#include <QAction>

#include <KMime/Message>

#include "accountwizard_debug.h"
#include <KAboutData>
#include <KLocalizedString>
#include <KMessageBox>
#include <PimCommon/PimUtil>
#include <qplatformdefs.h>

#include <KHelpMenu>

#include <gpgme++/engineinfo.h>
#include <gpgme++/error.h>

Dialog::Dialog(QWidget *parent)
    : KAssistantDialog(parent)
{
    resize(480, 600);
    mSetupManager = new SetupManager(this);
    const bool showPersonalDataPage = Global::typeFilter().size() == 1 && Global::typeFilter().at(0) == KMime::Message::mimeType();
    if (showPersonalDataPage) {
        // todo: don't ask these details based on a setting of the desktop file.
        auto *pdpage = new PersonalDataPage(this);
        addPage(pdpage, i18n("Provide personal data"));
        connect(pdpage, &PersonalDataPage::manualWanted, this, &Dialog::slotManualConfigWanted);
        if (!Global::assistant().isEmpty()) {
            pdpage->setHideOptionInternetSearch(true);
        }

        if (!GpgME::checkEngine(GpgME::OpenPGP)) {
            auto *cryptoPage = new CryptoPage(this);
            addPage(cryptoPage, i18n("Secure your Communication"));
        }
    }

    if (Global::assistant().isEmpty()) {
        auto *typePage = new TypePage(this);
        connect(typePage->treeview(), &QAbstractItemView::doubleClicked, this, &Dialog::slotNextPage);
#ifndef ACCOUNTWIZARD_NO_GHNS
        connect(typePage, &TypePage::ghnsWanted, this, &Dialog::slotGhnsWanted);
#endif
        mTypePage = addPage(typePage, i18n("Select Account Type"));
        setAppropriate(mTypePage, false);

#ifndef ACCOUNTWIZARD_NO_GHNS
        auto *ppage = new ProviderPage(this);
        connect(typePage, &TypePage::ghnsWanted, ppage, &ProviderPage::startFetchingData);
        connect(ppage->treeview(), &QAbstractItemView::doubleClicked, this, &Dialog::slotNextPage);
        connect(ppage, &ProviderPage::ghnsNotWanted, this, &Dialog::slotGhnsNotWanted);
        mProviderPage = addPage(ppage, i18n("Select Provider"));
        setAppropriate(mProviderPage, false);
#endif
    }

    auto *loadPage = new LoadPage(this);
    mLoadPage = addPage(loadPage, i18n("Loading Assistant"));
    setAppropriate(mLoadPage, false);
    loadPage->exportObject(this, QStringLiteral("Dialog"));
    loadPage->exportObject(mSetupManager, QStringLiteral("SetupManager"));
    loadPage->exportObject(new ServerTest(this), QStringLiteral("ServerTest"));
    connect(loadPage, &LoadPage::aboutToStart, this, &Dialog::clearDynamicPages);

    auto *setupPage = new SetupPage(this);
    mLastPage = addPage(setupPage, i18n("Setting up Account"));
    mSetupManager->setSetupPage(setupPage);

    slotManualConfigWanted(!showPersonalDataPage);

    Page *page = qobject_cast<Page *>(currentPage()->widget());
    page->enterPageNext();
    // TODO ? necessary ???
    Q_EMIT page->pageEnteredNext();
    connect(button(QDialogButtonBox::Cancel), &QPushButton::clicked, this, &Dialog::reject);
    auto *helpMenu = new KHelpMenu(this, KAboutData::applicationData(), true);
    // Initialize menu
    QMenu *menu = helpMenu->menu();
    helpMenu->action(KHelpMenu::menuAboutApp)->setIcon(QIcon::fromTheme(QStringLiteral("akonadi")));
    disconnect(helpMenu->action(KHelpMenu::menuHelpContents), nullptr, helpMenu, nullptr);
    connect(helpMenu->action(KHelpMenu::menuHelpContents), &QAction::triggered, this, &Dialog::slotOpenHelp);
    button(QDialogButtonBox::Help)->setMenu(menu);
}

KPageWidgetItem *Dialog::addPage(Page *page, const QString &title)
{
    KPageWidgetItem *item = KAssistantDialog::addPage(page, title);
    connect(page, &Page::leavePageNextOk, this, &Dialog::slotNextOk);
    connect(page, &Page::leavePageBackOk, this, &Dialog::slotBackOk);
    page->setPageWidgetItem(item);
    return item;
}

void Dialog::slotNextPage()
{
    next();
}

void Dialog::next()
{
    Page *page = qobject_cast<Page *>(currentPage()->widget());
    page->leavePageNext();
    page->leavePageNextRequested();
}

void Dialog::slotNextOk()
{
    Page *page = qobject_cast<Page *>(currentPage()->widget());
    Q_EMIT page->pageLeftNext();
    KAssistantDialog::next();
    page = qobject_cast<Page *>(currentPage()->widget());
    page->enterPageNext();
    Q_EMIT page->pageEnteredNext();
}

void Dialog::back()
{
    Page *page = qobject_cast<Page *>(currentPage()->widget());
    page->leavePageBack();
    page->leavePageBackRequested();
}

void Dialog::slotBackOk()
{
    Page *page = qobject_cast<Page *>(currentPage()->widget());
    Q_EMIT page->pageLeftBack();
    KAssistantDialog::back();
    page = qobject_cast<Page *>(currentPage()->widget());
    page->enterPageBack();
    Q_EMIT page->pageEnteredBack();
}

QObject *Dialog::addPage(const QString &uiFile, const QString &title)
{
    qCDebug(ACCOUNTWIZARD_LOG) << uiFile;
    auto *page = new DynamicPage(Global::assistantBasePath() + uiFile, this);
    connect(page, &DynamicPage::leavePageNextOk, this, &Dialog::slotNextOk);
    connect(page, &DynamicPage::leavePageBackOk, this, &Dialog::slotBackOk);
    KPageWidgetItem *item = insertPage(mLastPage, page, title);
    page->setPageWidgetItem(item);
    mDynamicPages.push_back(item);
    return page;
}

void Dialog::slotManualConfigWanted(bool show)
{
    if (mTypePage) {
        setAppropriate(mTypePage, show);
    }
    setAppropriate(mLoadPage, show);
}

#ifndef ACCOUNTWIZARD_NO_GHNS
void Dialog::slotGhnsWanted()
{
    Q_ASSERT(mProviderPage);
    setAppropriate(mProviderPage, true);
    setCurrentPage(mProviderPage);
}

void Dialog::slotGhnsNotWanted()
{
    Q_ASSERT(mProviderPage);
    setAppropriate(mProviderPage, false);
}

#endif

SetupManager *Dialog::setupManager()
{
    return mSetupManager;
}

void Dialog::clearDynamicPages()
{
    const auto dynamicPages = mDynamicPages;
    for (int i = 0, total = dynamicPages.count(); i < total; ++i) {
        KPageWidgetItem *item = mDynamicPages.at(i);
        removePage(item);
    }
    mDynamicPages.clear();
}

void Dialog::reject()
{
    connect(mSetupManager, &SetupManager::rollbackComplete, this, [this]() {
        KAssistantDialog::reject();
    });
    mSetupManager->requestRollback();
}

void Dialog::slotOpenHelp()
{
    PimCommon::Util::invokeHelp(QStringLiteral("kmail2/getting-started.html"), QStringLiteral("account-wizard"));
}
