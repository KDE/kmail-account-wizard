/*
    SPDX-FileCopyrightText: 2023-2024 Laurent Montel <montel.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QObject>

class SetupBase : public QObject
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
