// SPDX-FileCopyrightText: 2014 Sandro Knauß <knauss@kolabsys.com>
// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschan@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "setupautoconfigkolabmail.h"
#include "ispdb/autoconfigkolabmail.h"

SetupAutoconfigKolabMail::SetupAutoconfigKolabMail(QObject *parent)
    : IspdbHelper(parent, new AutoconfigKolabMail(this))
{
}
