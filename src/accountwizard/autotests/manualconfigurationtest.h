/*
    SPDX-FileCopyrightText: 2023-2024 Laurent Montel <montel.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/
#pragma once

#include <QObject>

class ManualConfigurationTest : public QObject
{
    Q_OBJECT
public:
    explicit ManualConfigurationTest(QObject *parent = nullptr);
    ~ManualConfigurationTest() override = default;

private Q_SLOTS:
    void shouldHaveDefaultValues();
    void shouldAssignEmail();
    void createResource();
};
