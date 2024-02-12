/*
    SPDX-FileCopyrightText: 2024 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "identitybase.h"

class IdentityImpl : public IdentityBase
{
    Q_OBJECT
public:
    explicit IdentityImpl(QObject *parent = nullptr);
    ~IdentityImpl() override;

    void createNewIdentity() override;
};
