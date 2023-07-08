/*
    SPDX-FileCopyrightText: 2023 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "setupbase.h"

SetupBase::SetupBase(QObject *parent)
    : QObject{parent}
{
}

SetupBase::~SetupBase()
{
}

#include "moc_setupbase.cpp"
