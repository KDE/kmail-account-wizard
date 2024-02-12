/*
    SPDX-FileCopyrightText: 2023-2024 Laurent Montel <montel.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "setupbase.h"

SetupBase::SetupBase(QObject *parent)
    : QObject{parent}
{
}

SetupBase::~SetupBase() = default;

#include "moc_setupbase.cpp"
