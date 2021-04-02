/*
    SPDX-FileCopyrightText: 2014 Sandro Knauß <knauss@kolabsys.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "ispdbhelper.h"

class SetupAutoconfigKolabMail : public IspdbHelper
{
    Q_OBJECT
public:
    /** Constructor */
    explicit SetupAutoconfigKolabMail(QObject *parent = nullptr);
};
