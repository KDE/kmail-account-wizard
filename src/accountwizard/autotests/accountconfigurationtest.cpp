/*
    SPDX-FileCopyrightText: 2023-2024 Laurent Montel <montel.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "accountconfigurationtest.h"
#include "accountconfigurationbase.h"
#include <QSignalSpy>
#include <QTest>
QTEST_MAIN(AccountConfigurationTest)
class ManualConfigurationImplTest : public AccountConfigurationBase
{
public:
    explicit ManualConfigurationImplTest(QObject *parent = nullptr)
        : AccountConfigurationBase(parent)
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

AccountConfigurationTest::AccountConfigurationTest(QObject *parent)
    : QObject{parent}
{
}

void AccountConfigurationTest::shouldHaveDefaultValues()
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

    QCOMPARE(w.incomingProtocols().count(), 3);
    QCOMPARE(w.securityProtocols().count(), 3);
    QCOMPARE(w.authenticationProtocols().count(), 7);
}

void AccountConfigurationTest::shouldAssignEmail()
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

void AccountConfigurationTest::createResource()
{
    // TODO
    ManualConfigurationImplTest w;
    w.createManualAccount();
    // TODO
}

void AccountConfigurationTest::createResource_data()
{
    QTest::addColumn<Resource::ResourceInfo>("resourceInfo");

    // TODO
}

#include "moc_accountconfigurationtest.cpp"
