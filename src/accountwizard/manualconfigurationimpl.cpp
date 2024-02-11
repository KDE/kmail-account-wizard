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

void ManualConfigurationImpl::generateResource(const Resource::ResourceInfo &info)
{
    auto resource = new Resource(this);
    resource->setResourceInfo(std::move(info));

    connect(resource, &Resource::info, this, &ManualConfigurationBase::info);
    connect(resource, &Resource::finished, this, &ManualConfigurationBase::finished);
    connect(resource, &Resource::error, this, &ManualConfigurationBase::error);
    resource->createResource();
}

#include "moc_manualconfigurationimpl.cpp"
