/*
    SPDX-FileCopyrightText: 2023-2024 Laurent Montel <montel.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "manualconfigurationtest.h"
#include "manualconfigurationbase.h"
#include <QSignalSpy>
#include <QTest>
QTEST_MAIN(ManualConfigurationTest)
class ManualConfigurationImplTest : public ManualConfigurationBase
{
public:
    explicit ManualConfigurationImplTest(QObject *parent = nullptr)
        : ManualConfigurationBase(parent)
    {
    }

protected:
    void createTransport() override
    {
        // TODO
    }
    void generateResource(const Resource::ResourceInfo &info) override
    {
        mResourceInfo = info;
    }

private:
    Resource::ResourceInfo mResourceInfo;
    QString mTransportInfo;
};

ManualConfigurationTest::ManualConfigurationTest(QObject *parent)
    : QObject{parent}
{
}

void ManualConfigurationTest::shouldHaveDefaultValues()
{
    ManualConfigurationImplTest w;
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

void ManualConfigurationTest::shouldAssignEmail()
{
    ManualConfigurationImplTest w;
    QSignalSpy incomingHostNameChanged(&w, &ManualConfigurationImplTest::incomingHostNameChanged);
    QSignalSpy outgoingHostNameChanged(&w, &ManualConfigurationImplTest::outgoingHostNameChanged);
    QSignalSpy incomingUserNameChanged(&w, &ManualConfigurationImplTest::incomingUserNameChanged);
    QSignalSpy outgoingUserNameChanged(&w, &ManualConfigurationImplTest::outgoingUserNameChanged);

    w.setEmail(QStringLiteral("foo@kde.org"));
    QCOMPARE(w.incomingHostName(), QStringLiteral("kde.org"));
    QCOMPARE(w.outgoingHostName(), QStringLiteral("kde.org"));
    QCOMPARE(w.incomingUserName(), QStringLiteral("foo@kde.org"));
    QCOMPARE(w.outgoingUserName(), QStringLiteral("foo@kde.org"));
    QCOMPARE(incomingHostNameChanged.count(), 1);
    QCOMPARE(outgoingHostNameChanged.count(), 1);
    QCOMPARE(incomingUserNameChanged.count(), 1);
    QCOMPARE(outgoingUserNameChanged.count(), 1);
}

#include "moc_manualconfigurationtest.cpp"
