/*
    SPDX-FileCopyrightText: 2023-2026 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/
#pragma once

#include <KIdentityManagementCore/IdentityManager>
#include <QObject>

class AccountConfigurationTest : public QObject
{
    Q_OBJECT
public:
    explicit AccountConfigurationTest(QObject *parent = nullptr);
    ~AccountConfigurationTest() override = default;

private Q_SLOTS:
    void init();
    void shouldHaveDefaultValues();
    void shouldAssignEmail();
    void createResource();
    void createResource_data();

private:
    std::unique_ptr<KIdentityManagementCore::IdentityManager> mManager;
};
