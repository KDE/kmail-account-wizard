/*
    SPDX-FileCopyrightText: 2010 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "dialog.h"
#include "global.h"

#include <QObject>
#include <QStringList>

class AccountWizard : public QObject
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.akonadi.AccountWizard")

public:
    explicit AccountWizard(QObject *parent = nullptr);

public Q_SLOTS:
    void run(const QStringList &types, QWidget *parent);
};

