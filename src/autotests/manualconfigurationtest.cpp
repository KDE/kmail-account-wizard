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

    QVERIFY(!w.incomingProtocols().isEmpty());
    QVERIFY(!w.securityProtocols().isEmpty());

    QCOMPARE(w.currentIncomingProtocol(), 0);
    QCOMPARE(w.currentIncomingSecurityProtocol(), 2);
    QCOMPARE(w.currentIncomingAuthenticationProtocol(), 0);
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
    ManualConfigurationImplTest w;
    QSignalSpy incomingHostNameChanged(&w, &ManualConfigurationImplTest::incomingHostNameChanged);
    QSignalSpy outgoingHostNameChanged(w.mailTransport(), &MailTransport::Transport::hostChanged);
    QSignalSpy incomingUserNameChanged(&w, &ManualConfigurationImplTest::incomingUserNameChanged);
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
    ManualConfigurationImplTest w;
    // w.save();
    //  TODO
}

void ManualConfigurationTest::createResource_data()
{
    QTest::addColumn<Resource::ResourceInfo>("resourceInfo");

    // TODO
}

#include "moc_manualconfigurationtest.cpp"
