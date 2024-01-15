/*
    SPDX-FileCopyrightText: 2024 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "identitybasetest.h"
#include "identitybase.h"
#include <QTest>
QTEST_MAIN(IdentityBaseTest)
IdentityBaseTest::IdentityBaseTest(QObject *parent)
    : QObject{parent}
{
}

IdentityBaseTest::~IdentityBaseTest() = default;
