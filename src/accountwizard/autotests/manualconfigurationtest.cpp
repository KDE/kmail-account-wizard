/*
    SPDX-FileCopyrightText: 2023-2024 Laurent Montel <montel.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

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
    QCOMPARE(w.currentIncomingAuthenticationProtocol(), 0);
    QCOMPARE(w.currentOutgoingAuthenticationProtocol(), 0);

    QCOMPARE(w.incomingPort(), 995);
    QCOMPARE(w.outgoingPort(), 465);
    QVERIFY(!w.disconnectedModeEnabled());
}

#include "moc_manualconfigurationtest.cpp"
