/*
    SPDX-FileCopyrightText: 2023-2024 Laurent Montel <montel.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "setupmanagertest.h"
#include "setupmanager.h"
#include <QTest>
QTEST_MAIN(SetupManagerTest)
SetupManagerTest::SetupManagerTest(QObject *parent)
    : QObject{parent}
{
}

void SetupManagerTest::shouldHaveDefaultValues()
{
    SetupManager w;
    // QVERIFY(w.fullName().isEmpty());
    // QVERIFY(w.email().isEmpty());
    // QVERIFY(w.password().isEmpty());

    QVERIFY(w.configurationModel());
}

#include "moc_setupmanagertest.cpp"
