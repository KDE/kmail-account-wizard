/*
    SPDX-FileCopyrightText: 2023-2025 Laurent Montel <montel@kde.org>
    SPDX-FileCopyrightText: 2024 Carl Schwan <carl@carlschwan.eu>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "accountconfigurationtest.h"
#include "accountconfiguration.h"
#include "consolelog.h"

#include <QSignalSpy>
#include <QStandardPaths>
#include <QTest>

#include <Akonadi/AgentManager>
#include <MailTransport/TransportManager>

QTEST_MAIN(AccountConfigurationTest)

using namespace Qt::Literals::StringLiterals;
using namespace KIdentityManagementCore;
using namespace Akonadi;

namespace
{

const auto i1Name = QStringLiteral("Test1");
const auto i1Email = QStringLiteral("firstname.lastname@example.com");

void cleanupIdentities(std::unique_ptr<IdentityManager> &manager)
{
    QVERIFY(manager);
    // It is picking up identities from older tests sometimes, so cleanup
    // Note this will always leave at least one identity -- remove later
    while (manager->identities().count() > 1) {
        QVERIFY(manager->removeIdentity(manager->identities().at(0)));
        manager->commit();
    }
}
}

AccountConfigurationTest::AccountConfigurationTest(QObject *parent)
    : QObject{parent}
{
}

void AccountConfigurationTest::init()
{
    QStandardPaths::setTestModeEnabled(true);
    mManager = std::make_unique<IdentityManager>();

    cleanupIdentities(mManager);
    QCOMPARE(mManager->identities().count(), 1); // Can't remove all identities

    {
        auto &i = mManager->newFromScratch(QStringLiteral("Test2"));
        i.setPrimaryEmailAddress(QStringLiteral("test@test.de"));
    }

    // Remove the first identity, which we couldn't remove above
    QVERIFY(mManager->removeIdentity(mManager->identities().at(0)));

    mManager->commit();
    QCOMPARE(mManager->identities().count(), 1);
}

void AccountConfigurationTest::shouldHaveDefaultValues()
{
    AccountConfiguration w(mManager.get());
    QVERIFY(w.incomingHostName().isEmpty());
    QVERIFY(w.incomingUserName().isEmpty());

    QCOMPARE(w.incomingProtocol(), AccountConfiguration::IncomingProtocol::IMAP);
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

void AccountConfigurationTest::shouldAssignEmail()
{
    AccountConfiguration w(mManager.get());
    QSignalSpy incomingHostNameChanged(&w, &AccountConfiguration::incomingHostNameChanged);
    QSignalSpy outgoingHostNameChanged(w.mailTransport(), &MailTransport::Transport::hostChanged);
    QSignalSpy incomingUserNameChanged(&w, &AccountConfiguration::incomingUserNameChanged);
    QSignalSpy outgoingUserNameChanged(w.mailTransport(), &MailTransport::Transport::userNameChanged);

    w.setEmail(u"foo@kde.org"_s);

    QCOMPARE(w.incomingHostName(), u"kde.org"_s);
    QCOMPARE(w.mailTransport()->host(), u"kde.org"_s);
    QCOMPARE(w.incomingUserName(), u"foo@kde.org"_s);
    QCOMPARE(w.mailTransport()->userName(), u"foo@kde.org"_s);
    QCOMPARE(incomingHostNameChanged.count(), 1);
    QCOMPARE(outgoingHostNameChanged.count(), 1);
    QCOMPARE(incomingUserNameChanged.count(), 1);
    QCOMPARE(outgoingUserNameChanged.count(), 1);
}

void AccountConfigurationTest::createResource()
{
    // Ensure nothing is there before adding the email configurations
    QCOMPARE(KIdentityManagementCore::IdentityManager::self()->identities().count(), 1);
    QVERIFY(MailTransport::TransportManager::self()->transports().isEmpty());

    QFETCH(QString, email);
    QFETCH(QString, fullName);
    QFETCH(QString, password);
    QFETCH(AccountConfiguration::IncomingProtocol, protocol);
    QFETCH(QString, incomingHostName);
    QFETCH(MailTransport::Transport::EnumEncryption, incomingEncryption);
    QFETCH(MailTransport::Transport::EnumAuthenticationType, incomingAuth);
    QFETCH(QString, outgoingHostName);
    QFETCH(MailTransport::Transport::EnumEncryption, outgoingEncryption);
    QFETCH(MailTransport::Transport::EnumAuthenticationType, outgoingAuth);

    AccountConfiguration w(mManager.get());
    w.setEmail(email);
    w.setIncomingProtocol(protocol);
    w.identity().setFullName(fullName);
    w.setIncomingHostName(incomingHostName);
    w.setIncomingSecurityProtocol(incomingEncryption);
    w.setIncomingAuthenticationProtocol(incomingAuth);
    w.mailTransport()->setHost(outgoingHostName);
    w.mailTransport()->setEncryption(outgoingEncryption);
    w.mailTransport()->setAuthenticationType(outgoingAuth);

    QCOMPARE(w.email(), u"foo@kde.org"_s);
    QCOMPARE(w.identity().fullName(), fullName);

    QCOMPARE(w.incomingProtocol(), protocol);
    QCOMPARE(w.incomingHostName(), u"imap.kde.org"_s);
    QCOMPARE(w.incomingUserName(), u"foo@kde.org"_s);
    QCOMPARE(w.incomingSecurityProtocol(), MailTransport::Transport::EnumEncryption::TLS);
    QCOMPARE(w.incomingAuthenticationProtocol(), MailTransport::Transport::EnumAuthenticationType::CRAM_MD5);

    QCOMPARE(w.mailTransport()->host(), u"smtp.kde.org"_s);
    QCOMPARE(w.mailTransport()->userName(), u"foo@kde.org"_s);
    QCOMPARE(w.mailTransport()->port(), 587);
    QCOMPARE(w.mailTransport()->storePassword(), true);
    QCOMPARE(w.mailTransport()->encryption(), MailTransport::Transport::EnumEncryption::TLS);
    QCOMPARE(w.mailTransport()->authenticationType(), MailTransport::Transport::EnumAuthenticationType::CRAM_MD5);

    auto consoleLog = new ConsoleLog();

    QSignalSpy agentAdded(AgentManager::self(), &AgentManager::instanceAdded);
    w.save(consoleLog);

    QCOMPARE(mManager->identities().count(), 2);
    QCOMPARE(mManager->identityForUoid(w.identity().uoid()).fullName(), fullName);
    QCOMPARE(mManager->identityForAddress(email).fullName(), fullName);

    QCOMPARE(MailTransport::TransportManager::self()->transports().count(), 1);

    agentAdded.wait();
    QCOMPARE(agentAdded.count(), 1);

    AgentInstance agent = agentAdded.takeFirst().at(0).value<Akonadi::AgentInstance>();
    QCOMPARE(agent.type().identifier(), u"akonadi_imap_resource"_s);
    QVERIFY(agent.isValid());
}

void AccountConfigurationTest::createResource_data()
{
    QTest::addColumn<QString>("email");
    QTest::addColumn<QString>("fullName");
    QTest::addColumn<QString>("password");
    QTest::addColumn<AccountConfiguration::IncomingProtocol>("protocol");
    QTest::addColumn<QString>("incomingHostName");
    QTest::addColumn<MailTransport::Transport::EnumEncryption>("incomingEncryption");
    QTest::addColumn<MailTransport::Transport::EnumAuthenticationType>("incomingAuth");
    QTest::addColumn<QString>("outgoingHostName");
    QTest::addColumn<MailTransport::Transport::EnumEncryption>("outgoingEncryption");
    QTest::addColumn<MailTransport::Transport::EnumAuthenticationType>("outgoingAuth");
    QTest::addColumn<QString>("resourceIdentifier");

    QTest::addRow("imap") << u"foo@kde.org"_s << u"Foo Bar"_s << u"password"_s << AccountConfiguration::IncomingProtocol::IMAP << u"imap.kde.org"_s
                          << MailTransport::Transport::EnumEncryption::TLS << MailTransport::Transport::EnumAuthenticationType::CRAM_MD5 << u"smtp.kde.org"_s
                          << MailTransport::Transport::EnumEncryption::TLS << MailTransport::Transport::EnumAuthenticationType::CRAM_MD5
                          << "akonadi_imap_resource";
}

#include "moc_accountconfigurationtest.cpp"
