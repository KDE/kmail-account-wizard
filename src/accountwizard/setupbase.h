/*
    SPDX-FileCopyrightText: 2023 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once
#include "libaccountwizard_private_export.h"
#include <QObject>

class LIBACCOUNTWIZARD_TESTS_EXPORT SetupBase : public QObject
{
    Q_OBJECT
public:
    explicit SetupBase(QObject *parent = nullptr);
    ~SetupBase() override;

Q_SIGNALS:
    void error(const QString &msg);
    void info(const QString &msg);
    void finished(const QString &msg);
};
