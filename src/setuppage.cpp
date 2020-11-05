/*
    SPDX-FileCopyrightText: 2009 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "setuppage.h"
#include <QIcon>
#include <QStandardItemModel>

SetupPage::SetupPage(KAssistantDialog *parent)
    : Page(parent)
    , m_msgModel(new QStandardItemModel(this))
{
    ui.setupUi(this);
    ui.detailView->setModel(m_msgModel);
    connect(ui.detailsButton, &QPushButton::clicked, this, &SetupPage::detailsClicked);
}

void SetupPage::enterPageNext()
{
    ui.stackWidget->setCurrentIndex(0);
}

void SetupPage::detailsClicked()
{
    ui.stackWidget->setCurrentIndex(1);
}

void SetupPage::addMessage(SetupPage::MessageType type, const QString &msg)
{
    auto *item = new QStandardItem;
    item->setText(msg);
    item->setEditable(false);
    switch (type) {
    case Success:
        item->setIcon(QIcon::fromTheme(QStringLiteral("dialog-ok")));
        break;
    case Info:
        item->setIcon(QIcon::fromTheme(QStringLiteral("dialog-information")));
        break;
    case Error:
        item->setIcon(QIcon::fromTheme(QStringLiteral("dialog-error")));
        break;
    }
    m_msgModel->appendRow(item);
}

void SetupPage::setStatus(const QString &msg)
{
    ui.progressLabel->setText(msg);
}

void SetupPage::setProgress(int percent)
{
    ui.progressBar->setValue(percent);
}
