/*
    SPDX-FileCopyrightText: 2024 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "accountconfigurationbase.h"
#include "resource.h"

class AccountConfigurationImpl : public AccountConfigurationBase
{
    Q_OBJECT
public:
    explicit AccountConfigurationImpl(QObject *parent = nullptr);
    ~AccountConfigurationImpl() override;

protected:
    LIBACCOUNTWIZARD_NO_EXPORT void createTransport() override;
    LIBACCOUNTWIZARD_NO_EXPORT void generateResource(const Resource::ResourceInfo &info) override;
};
