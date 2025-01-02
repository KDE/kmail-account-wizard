// SPDX-FileCopyrightText: 2023-2025 Laurent Montel <montel@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "resourcetest.h"
#include "resource.h"
#include <QTest>
QTEST_MAIN(ResourceTest)
ResourceTest::ResourceTest(QObject *parent)
    : QObject{parent}
{
}

void ResourceTest::shouldHaveDefaultValues()
{
    Resource w({});
    QVERIFY(w.name().isEmpty());
    QVERIFY(w.settings().isEmpty());
}

#include "moc_resourcetest.cpp"
