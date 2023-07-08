/*
    SPDX-FileCopyrightText: 2023 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QObject>

class SetupManagerTest : public QObject
{
    Q_OBJECT
public:
    explicit SetupManagerTest(QObject *parent = nullptr);
    ~SetupManagerTest() override = default;
private Q_SLOTS:
    void shouldHaveDefaultValues();
};
