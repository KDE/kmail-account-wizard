/*
    SPDX-FileCopyrightText: 2024 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "manualconfigurationimpl.h"

ManualConfigurationImpl::ManualConfigurationImpl(QObject *parent)
    : ManualConfigurationBase{parent}
{
}

ManualConfigurationImpl::~ManualConfigurationImpl() = default;

void ManualConfigurationImpl::createResource()
{
}

void ManualConfigurationImpl::createTransport()
{
}

#include "moc_manualconfigurationimpl.cpp"
