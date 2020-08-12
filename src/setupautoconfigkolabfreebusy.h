/*
    SPDX-FileCopyrightText: 2014 Sandro Knauß <knauss@kolabsys.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef SETUPAUTOCONFIGKOLABFREEBUSY_H
#define SETUPAUTOCONFIGKOLABFREEBUSY_H

#include "setupobject.h"

class AutoconfigKolabFreebusy;

class SetupAutoconfigKolabFreebusy : public SetupObject
{
    Q_OBJECT
public:
    /** Constructor */
    explicit SetupAutoconfigKolabFreebusy(QObject *parent = nullptr);
    ~SetupAutoconfigKolabFreebusy() override;

    void create() override;
    void destroy() override;

public Q_SLOTS:
    Q_SCRIPTABLE void fillFreebusyServer(int i, QObject *) const;
    Q_SCRIPTABLE int countFreebusyServers() const;

    Q_SCRIPTABLE void start();

    Q_SCRIPTABLE void setEmail(const QString &);
    Q_SCRIPTABLE void setPassword(const QString &);

Q_SIGNALS:
    void ispdbFinished(bool);

private:
    void onIspdbFinished(bool);

    AutoconfigKolabFreebusy *mIspdb = nullptr;
};

#endif
