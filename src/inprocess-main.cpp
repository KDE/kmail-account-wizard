/*
    SPDX-FileCopyrightText: 2010 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "inprocess-main.h"

AccountWizard::AccountWizard(QObject *parent)
    : QObject(parent)
{
}

void AccountWizard::run(const QStringList &types, QWidget *parent)
{
    if (!types.isEmpty()) {
        Global::setTypeFilter(types);
    }
    Dialog dlg(parent);
    dlg.exec();
}
