/*
    SPDX-FileCopyrightText: 2023-2024 Laurent Montel <montel.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/
#pragma once

#include <KIdentityManagementCore/IdentityManager>
#include <QObject>

class ManualConfigurationTest : public QObject
{
    Q_OBJECT
public:
    explicit ManualConfigurationTest(QObject *parent = nullptr);
    ~ManualConfigurationTest() override = default;

private Q_SLOTS:
    void init();
    void shouldHaveDefaultValues();
    void shouldAssignEmail();
    void createResource();
    void createResource_data();

private:
    std::unique_ptr<KIdentityManagementCore::IdentityManager> mManager;
};
