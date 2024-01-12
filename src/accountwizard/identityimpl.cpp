/*
    SPDX-FileCopyrightText: 2024 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "identityimpl.h"

IdentityImpl::IdentityImpl(QObject *parent)
    : IdentityBase{parent}
{
}

IdentityImpl::~IdentityImpl() = default;

void IdentityImpl::create()
{
    // TODO
}

#include "moc_identityimpl.cpp"
