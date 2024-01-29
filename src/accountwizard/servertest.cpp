/*
    SPDX-FileCopyrightText: 2010 Tom Albers <toma@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "servertest.h"
#include <MailTransport/ServerTest>
#include <MailTransport/Transport>

#include "accountwizard_debug.h"

ServerTest::ServerTest(QObject *parent)
    : QObject(parent)
    , m_serverTest(new MailTransport::ServerTest(nullptr))
{
    qCDebug(ACCOUNTWIZARD_LOG) << "Welcome!";
    connect(m_serverTest, &MailTransport::ServerTest::finished, this, &ServerTest::testFinished);
}

ServerTest::~ServerTest()
{
    delete m_serverTest;
}

void ServerTest::test(const QString &server, const QString &protocol)
{
    qCDebug(ACCOUNTWIZARD_LOG) << server << protocol;
    m_serverTest->setServer(server);
    m_serverTest->setProtocol(protocol);
    if (protocol == QLatin1StringView("submission")) {
        m_serverTest->setProtocol(QStringLiteral("smtp"));
        m_serverTest->setPort(MailTransport::Transport::EnumEncryption::None, 587);
        m_serverTest->setPort(MailTransport::Transport::EnumEncryption::SSL, 0); // No ssl port for submission
    }
    m_serverTest->start();
}

void ServerTest::testFinished(const QVector<int> &list)
{
    qCDebug(ACCOUNTWIZARD_LOG) << "types: " << list;
    if (list.contains(MailTransport::Transport::EnumEncryption::TLS)) {
        Q_EMIT testResult(QStringLiteral("tls"));
    } else if (list.contains(MailTransport::Transport::EnumEncryption::SSL)) {
        Q_EMIT testResult(QStringLiteral("ssl"));
    } else {
        Q_EMIT testFail();
    }
}

#include "moc_servertest.cpp"
