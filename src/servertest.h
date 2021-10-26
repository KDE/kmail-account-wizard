/*
    SPDX-FileCopyrightText: 2010 Tom Albers <toma@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QObject>

namespace MailTransport
{
class ServerTest;
}

class ServerTest : public QObject
{
    Q_OBJECT
public:
    explicit ServerTest(QObject *parent);
    ~ServerTest() override;

public Q_SLOTS:
    /* @p protocol being 'imap' 'smtp' or 'pop3' */
    Q_SCRIPTABLE void test(const QString &server, const QString &protocol);

Q_SIGNALS:
    /* returns the advised setting, @p result begin 'ssl' 'tls' or 'none'. */
    void testResult(const QString &result);

    /* returns if no connection is possible, test failed. */
    void testFail();

private Q_SLOTS:
    void testFinished(const QVector<int> &list);

private:
    MailTransport::ServerTest *m_serverTest = nullptr;
};

