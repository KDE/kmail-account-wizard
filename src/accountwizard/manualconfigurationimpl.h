/*
    SPDX-FileCopyrightText: 2024 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "manualconfigurationbase.h"
#include "resource.h"

class ManualConfigurationImpl : public ManualConfigurationBase
{
    Q_OBJECT
public:
    explicit ManualConfigurationImpl(QObject *parent = nullptr);
    ~ManualConfigurationImpl() override;

protected:
    LIBACCOUNTWIZARD_NO_EXPORT void createTransport() override;
    LIBACCOUNTWIZARD_NO_EXPORT void generateResource(const Resource::ResourceInfo &info) override;
};
