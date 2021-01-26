/*
    SPDX-FileCopyrightText: 2014 Sandro Knauß <knauss@kolabsys.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include <QDir>
#include <QObject>
#include <QTest>

#include <KConfig>
#include <KConfigGroup>
#include <QTemporaryFile>

#include "../ldap.h"
#include <KLDAP/LdapClientSearchConfig>

class TLdap : public Ldap
{
    Q_OBJECT
public:
    explicit TLdap(QObject *parent = nullptr)
        : Ldap(parent)
    {
        mTempFile.open();
        mTempFile.close();
        mConfig = new KConfig(mTempFile.fileName(), KConfig::SimpleConfig);
        // TODO m_clientSearchConfig->askForWallet(false);
    }

    ~TLdap() override
    {
        mTempFile.close();
        delete mConfig;
    }

    KConfig *config() const override
    {
        mConfig->reparseConfiguration();
        return mConfig;
    }

    QTemporaryFile mTempFile;
    KConfig *mConfig = nullptr;
};

class LdapTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testDestroy()
    {
#if 0 // TODO port it
        TLdap l;
        QFile::remove(l.mTempFile.fileName());
        QFile::copy(QStringLiteral(TEST_DATA_DIR) + QStringLiteral("/ldap.cfg"), l.mTempFile.fileName());
        KLDAP::LdapClientSearchConfig csc;
        csc.askForWallet(false);

        KConfigGroup group = l.config()->group("LDAP");
        int cSelHosts = group.readEntry("NumSelectedHosts", 0);
        int cHosts = group.readEntry("NumHosts", 0);
        QCOMPARE(cSelHosts, 3);
        QCOMPARE(cHosts, 1);
        QVector<KLDAP::LdapServer> selHosts;
        selHosts.reserve(cSelHosts);

        for (int i = 0; i < cSelHosts; ++i) {
            KLDAP::LdapServer server;
            csc.readConfig(server, group, i, true);
            selHosts.append(server);
        }
        QVector<KLDAP::LdapServer> hosts;
        hosts.reserve(cHosts);
        for (int i = 0; i < cHosts; ++i) {
            KLDAP::LdapServer server;
            csc.readConfig(server, group, i, false);
            hosts.append(server);
        }

        l.m_entry = 0;
        l.destroy();
        group = l.config()->group("LDAP");

        QCOMPARE(group.readEntry("NumSelectedHosts", 0), 2);
        QCOMPARE(group.readEntry("NumHosts", 0), 1);
        KLDAP::LdapServer server;
        csc.readConfig(server, group, 0, false);
        QCOMPARE(server.host(), hosts.at(0).host());

        csc.readConfig(server, group, 0, true);
        QCOMPARE(server.host(), selHosts.at(1).host());
        csc.readConfig(server, group, 1, true);
        QCOMPARE(server.host(), selHosts.at(2).host());
#endif
    }
};

QTEST_GUILESS_MAIN(LdapTest)

#include "ldaptest.moc"
