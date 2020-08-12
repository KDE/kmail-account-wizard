/*
    SPDX-FileCopyrightText: 2014 Sandro Knauß <knauss@kolabsys.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef SETUPAUTOCONFIGKOLABMAIL_H
#define SETUPAUTOCONFIGKOLABMAIL_H

#include "setupispdb.h"

class SetupAutoconfigKolabMail : public SetupIspdb
{
    Q_OBJECT
public:
    /** Constructor */
    explicit SetupAutoconfigKolabMail(QObject *parent = nullptr);
};

#endif
