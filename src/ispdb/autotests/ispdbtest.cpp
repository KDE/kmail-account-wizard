/*
    SPDX-FileCopyrightText: 2014 Sandro Knauß <knauss@kolabsys.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include <QObject>
#include <QDir>
#include <QTest>

#include "../ispdb.h"

class TIspdb : public Ispdb
{
public:
    void start(const QUrl &url)
    {
        setServerType(Ispdb::IspWellKnow);
        startJob(url);
    }
};

class IspdbTest : public QObject
{
    Q_OBJECT
public:
    Ispdb *execIspdb(const QString &file)
    {
        QDir dir(QStringLiteral(AUTOCONFIG_DATA_DIR));
        QString furl = QStringLiteral("file://%1/%2");

        QEventLoop loop;
        TIspdb *ispdb = new TIspdb();

        loop.connect(ispdb, SIGNAL(finished(bool)), SLOT(quit()));

        QUrl url(furl.arg(dir.path()).arg(file));
        ispdb->setEmail(QStringLiteral("john.doe@example.com"));
        ispdb->start(url);

        loop.exec();
        return ispdb;
    }

    void testServer(const Server &test, const Server &expected) const
    {
        QVERIFY(test.isValid());
        QCOMPARE(test.hostname, expected.hostname);
        QCOMPARE(test.port, expected.port);
        QCOMPARE(test.socketType, expected.socketType);
        QCOMPARE(test.username, expected.username);
        QCOMPARE(test.authentication, expected.authentication);
    }

    void testIdendity(const identity &test, const identity &expected) const
    {
        QVERIFY(test.isValid());
        QCOMPARE(test.name, expected.name);
        QCOMPARE(test.email, expected.email);
        QCOMPARE(test.organization, expected.organization);
        QCOMPARE(test.signature, expected.signature);
        QCOMPARE(test.isDefault(), expected.isDefault());
    }

private Q_SLOTS:
    void testParsing()
    {
        Ispdb *ispdb = execIspdb(QStringLiteral("autoconfig.xml"));

        Server s;
        identity i;

        s.hostname = QStringLiteral("imap.example.com");
        s.port = 993;
        s.socketType = Ispdb::SSL;
        s.authentication = Ispdb::CramMD5;
        s.username = QStringLiteral("john.doe@example.com");

        QCOMPARE(ispdb->imapServers().count(), 1);
        testServer(ispdb->imapServers().first(), s);

        s.hostname = QStringLiteral("smtp.example.com");
        s.port = 25;
        s.socketType = Ispdb::None;
        s.authentication = Ispdb::Plain;
        s.username = QStringLiteral("john.doe");
        QCOMPARE(ispdb->smtpServers().count(), 1);
        testServer(ispdb->smtpServers().first(), s);

        s.hostname = QStringLiteral("pop.example.com");
        s.port = 995;
        s.socketType = Ispdb::StartTLS;
        s.authentication = Ispdb::NTLM;
        s.username = QStringLiteral("example.com");
        QCOMPARE(ispdb->pop3Servers().count(), 1);
        testServer(ispdb->pop3Servers().first(), s);

        i.mDefault = true;
        i.name = QStringLiteral("John Doe");
        i.email = QStringLiteral("john.doe@example.com");
        i.organization = QStringLiteral("Example AG");
        i.signature = QStringLiteral("John Doe\n\
Head of World\n\
\n\
Example AG\n\
\n\
w: <a href=\"http://example.com\">http://example.com</a>");

        QCOMPARE(ispdb->identities().count(), 1);
        testIdendity(ispdb->identities().first(), i);
        QCOMPARE(ispdb->defaultIdentity(), 0);
    }
};

QTEST_MAIN(IspdbTest)

#include "ispdbtest.moc"
