/*
    SPDX-FileCopyrightText: 2024 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "accountconfigurationimpl.h"

AccountConfigurationImpl::AccountConfigurationImpl(QObject *parent)
    : AccountConfigurationBase{parent}
{
}

AccountConfigurationImpl::~AccountConfigurationImpl() = default;

void AccountConfigurationImpl::generateResource(const Resource::ResourceInfo &info)
{
    auto resource = new Resource(this);
    resource->setResourceInfo(std::move(info));

    connect(resource, &Resource::info, this, &AccountConfigurationBase::info);
    connect(resource, &Resource::finished, this, &AccountConfigurationBase::finished);
    connect(resource, &Resource::error, this, &AccountConfigurationBase::error);
    resource->createResource();
}

void AccountConfigurationImpl::createTransport()
{
    // Create outgoing account
    auto transport = new Transport(this);
    transport->setTransportInfo(std::move(createTransportInfo()));

    connect(transport, &Transport::info, this, &AccountConfigurationBase::info);
    connect(transport, &Transport::finished, this, &AccountConfigurationBase::finished);
    connect(transport, &Transport::error, this, &AccountConfigurationBase::error);
    transport->createTransport();
}

#include "moc_accountconfigurationimpl.cpp"
