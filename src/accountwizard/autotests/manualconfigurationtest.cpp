// SPDX-FileCopyrightText: 2023 Laurent Montel <montel@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "manualconfigurationtest.h"
#include "manualconfiguration.h"
#include <QTest>
QTEST_MAIN(ManualConfigurationTest)
ManualConfigurationTest::ManualConfigurationTest(QObject *parent)
    : QObject{parent}
{
}

void ManualConfigurationTest::shouldHaveDefaultValues()
{
    ManualConfiguration w;
    QVERIFY(w.incomingHostName().isEmpty());
    QVERIFY(w.incomingUserName().isEmpty());
    QVERIFY(w.outgoingHostName().isEmpty());
    QVERIFY(w.outgoingUserName().isEmpty());

    QVERIFY(!w.incomingProtocols().isEmpty());
    QVERIFY(!w.securityProtocols().isEmpty());
    QVERIFY(!w.authenticationProtocols().isEmpty());

    QCOMPARE(w.currentIncomingProtocol(), 0);
    QCOMPARE(w.currentIncomingSecurityProtocol(), 2);
    QCOMPARE(w.currentOutgoingSecurityProtocol(), 2);
    QCOMPARE(w.currentIncomingAuthenticationProtocols(), 0);
    QCOMPARE(w.currentOutgoingAuthenticationProtocols(), 0);

    QCOMPARE(w.incomingPort(), 995);
    QCOMPARE(w.outgoingPort(), 465);
}

#include "moc_manualconfigurationtest.cpp"
