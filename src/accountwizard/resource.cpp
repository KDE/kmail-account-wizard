/*
    SPDX-FileCopyrightText: 2023 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "resource.h"

Resource::Resource(const QString &resourceType, QObject *parent)
    : QObject{parent}
    , mTypeIdentifier(resourceType)
{
}

Resource::~Resource()
{
}
