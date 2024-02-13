/*
    SPDX-FileCopyrightText: 2023-2024 Laurent Montel <montel.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "manualconfigurationtest.h"
#include "manualconfiguration.h"

#include <QSignalSpy>
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

    QCOMPARE(w.incomingProtocol(), 0);
    QCOMPARE(w.incomingSecurityProtocol(), MailTransport::Transport::EnumEncryption::SSL);
    QCOMPARE(w.incomingAuthenticationProtocol(), MailTransport::Transport::EnumAuthenticationType::LOGIN);
    QCOMPARE(w.incomingPort(), 995);
    QVERIFY(!w.disconnectedModeEnabled());

    QVERIFY(w.mailTransport()->host().isEmpty());
    QVERIFY(w.mailTransport()->userName().isEmpty());
    QCOMPARE(w.mailTransport()->encryption(), MailTransport::Transport::EnumEncryption::SSL);
    QCOMPARE(w.mailTransport()->authenticationType(), MailTransport::Transport::EnumAuthenticationType::PLAIN);
    QCOMPARE(w.mailTransport()->port(), 587);
    QCOMPARE(w.mailTransport()->storePassword(), true);
}

void ManualConfigurationTest::shouldAssignEmail()
{
    ManualConfiguration w;
    QSignalSpy incomingHostNameChanged(&w, &ManualConfiguration::incomingHostNameChanged);
    QSignalSpy outgoingHostNameChanged(w.mailTransport(), &MailTransport::Transport::hostChanged);
    QSignalSpy incomingUserNameChanged(&w, &ManualConfiguration::incomingUserNameChanged);
    QSignalSpy outgoingUserNameChanged(w.mailTransport(), &MailTransport::Transport::userNameChanged);

    w.setEmail(QStringLiteral("foo@kde.org"));
    QCOMPARE(w.incomingHostName(), QStringLiteral("kde.org"));
    QCOMPARE(w.mailTransport()->host(), QStringLiteral("kde.org"));
    QCOMPARE(w.incomingUserName(), QStringLiteral("foo@kde.org"));
    QCOMPARE(w.mailTransport()->userName(), QStringLiteral("foo@kde.org"));
    QCOMPARE(incomingHostNameChanged.count(), 1);
    QCOMPARE(outgoingHostNameChanged.count(), 1);
    QCOMPARE(incomingUserNameChanged.count(), 1);
    QCOMPARE(outgoingUserNameChanged.count(), 1);
}

void ManualConfigurationTest::createResource()
{
    // TODO
    ManualConfiguration w;
    // w.save();
    //  TODO
}

void ManualConfigurationTest::createResource_data()
{
    QTest::addColumn<Resource::ResourceInfo>("resourceInfo");

    // TODO
}

#include "moc_manualconfigurationtest.cpp"
