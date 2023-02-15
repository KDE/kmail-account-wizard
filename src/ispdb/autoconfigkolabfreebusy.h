/*
 * SPDX-FileCopyrightText: 2014 Sandro Knauß <knauss@kolabsys.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#pragma once

#include "autoconfigkolabmail.h"

struct freebusy;

class AutoconfigKolabFreebusy : public AutoconfigKolabMail
{
    Q_OBJECT
public:
    /** Constructor */
    explicit AutoconfigKolabFreebusy(QObject *parent = nullptr);

    Q_REQUIRED_RESULT QHash<QString, freebusy> freebusyServers() const;

protected:
    void lookupInDb(bool auth, bool crypt) override;
    void parseResult(const QDomDocument &document) override;

private:
    freebusy createFreebusyServer(const QDomElement &n);

    QHash<QString, freebusy> mFreebusyServer;
};

struct freebusy {
    freebusy()
        : port(80)
        , socketType(Ispdb::None)
        , authentication(Ispdb::Plain)
    {
    }

    Q_REQUIRED_RESULT bool isValid() const
    {
        return port != -1;
    }

    QString hostname;
    QString username;
    QString password;
    QString path;
    int port;
    Ispdb::socketType socketType;
    Ispdb::authType authentication;
};

