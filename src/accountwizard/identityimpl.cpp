/*
    SPDX-FileCopyrightText: 2024 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "identityimpl.h"
#include "accountwizard_debug.h"
#include <KIdentityManagementCore/Identity>
#include <KIdentityManagementCore/IdentityManager>

IdentityImpl::IdentityImpl(QObject *parent)
    : IdentityBase{parent}
{
}

IdentityImpl::~IdentityImpl() = default;

void IdentityImpl::createNewIdentity()
{
    auto manager = KIdentityManagementCore::IdentityManager::self();
    manager->commit();
    if (!manager->setAsDefault(mIdentity->uoid())) {
        qCWarning(ACCOUNTWIZARD_LOG) << "Impossible to find identity";
    }
}

#include "moc_identityimpl.cpp"
