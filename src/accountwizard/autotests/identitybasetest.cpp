/*
    SPDX-FileCopyrightText: 2024 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "identitybasetest.h"
#include "identitybase.h"
#include <QTest>
QTEST_MAIN(IdentityBaseTest)

class IdentityTest : public IdentityBase
{
public:
    explicit IdentityTest(QObject *parent = nullptr)
        : IdentityBase(parent)
    {
    }

protected:
    void createNewIdentity() override
    {
        // TODO
    }
};

IdentityBaseTest::IdentityBaseTest(QObject *parent)
    : QObject{parent}
{
}

IdentityBaseTest::~IdentityBaseTest() = default;
