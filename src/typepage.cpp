/*
    SPDX-FileCopyrightText: 2009 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "typepage.h"

#include "accountwizard_debug.h"
#include <KConfigGroup>
#include <KDesktopFile>
#include <QIcon>
#include <QLineEdit>

#include "global.h"
#include <QDir>
#include <QMimeDatabase>
#include <QMimeType>
#include <QSortFilterProxyModel>
#include <QStandardPaths>

TypePage::TypePage(KAssistantDialog *parent)
    : Page(parent)
    , m_model(new QStandardItemModel(this))
{
    ui.setupUi(this);

#ifdef ACCOUNTWIZARD_NO_GHNS
    ui.ghnsButton->hide();
#endif

    mProxy = new QSortFilterProxyModel(this);
    mProxy->setSourceModel(m_model);
    mProxy->setFilterKeyColumn(-1);
    mProxy->setFilterCaseSensitivity(Qt::CaseInsensitive);

    ui.listView->setModel(mProxy);
    connect(ui.searchLine, &QLineEdit::textChanged, this, &TypePage::slotTextChanged);

    QStringList list;
    const QStringList dirs =
        QStandardPaths::locateAll(QStandardPaths::GenericDataLocation, QStringLiteral("akonadi/accountwizard/"), QStandardPaths::LocateDirectory);
    for (const QString &dir : dirs) {
        const QStringList directories = QDir(dir).entryList(QDir::AllDirs);
        for (const QString &directory : directories) {
            const QString fullPath = dir + QLatin1Char('/') + directory;
            const QStringList fileNames = QDir(fullPath).entryList(QStringList() << QStringLiteral("*.desktop"));
            list.reserve(fileNames.count());
            for (const QString &file : fileNames) {
                list.append(fullPath + QLatin1Char('/') + file);
            }
        }
    }

    const QStringList filter = Global::typeFilter();
    for (const QString &entry : std::as_const(list)) {
        KDesktopFile f(entry);
        qCDebug(ACCOUNTWIZARD_LOG) << entry << f.readName();
        const KConfig configWizard(entry);
        KConfigGroup grp(&configWizard, "Wizard");
        const QStringList lstType = grp.readEntry("Type", QStringList());
        if (lstType.isEmpty()) {
            qCDebug(ACCOUNTWIZARD_LOG) << QStringLiteral(" %1 doesn't contains specific type").arg(f.readName());
        }
        if (!filter.isEmpty()) {
            // stolen from agentfilterproxymodel
            bool found = false;
            for (const QString &mimeType : lstType) {
                if (filter.contains(mimeType)) {
                    found = true;
                    break;
                } else {
                    for (const QString &type : filter) {
                        QMimeDatabase db;
                        QMimeType typePtr = db.mimeTypeForName(type);
                        if (typePtr.isValid() && typePtr.inherits(mimeType)) {
                            found = true;
                            break;
                        }
                    }
                }
                if (found) {
                    break;
                }
            }
            if (!found) {
                continue;
            }
        }
        auto *item = new QStandardItem(f.readName());
        item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        item->setData(entry, Qt::UserRole);
        if (!f.readIcon().isEmpty()) {
            item->setData(QIcon::fromTheme(f.readIcon()), Qt::DecorationRole);
        }
        item->setData(f.readComment(), Qt::ToolTipRole);
        m_model->appendRow(item);
    }

    connect(ui.listView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &TypePage::selectionChanged);
    connect(ui.ghnsButton, &QPushButton::clicked, this, &TypePage::ghnsWanted);
}

void TypePage::slotTextChanged(const QString &text)
{
    mProxy->setFilterFixedString(text);
}

void TypePage::selectionChanged()
{
    if (ui.listView->selectionModel()->hasSelection()) {
        setValid(true);
    } else {
        setValid(false);
    }
}

void TypePage::leavePageNext()
{
    if (!ui.listView->selectionModel()->hasSelection()) {
        return;
    }
    const QModelIndex index = ui.listView->selectionModel()->selectedIndexes().at(0);
    Global::setAssistant(index.data(Qt::UserRole).toString());
}

QTreeView *TypePage::treeview() const
{
    return ui.listView;
}
