/*
 * SPDX-FileCopyrightText: 2014 Sandro Knauß <knauss@kolabsys.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef AUTOCONFIGKOLABMAIL_H
#define AUTOCONFIGKOLABMAIL_H

#include "ispdb.h"

class AutoconfigKolabMail : public Ispdb
{
    Q_OBJECT
public:
    /** Constructor */
    explicit AutoconfigKolabMail(QObject *parent = nullptr);

    void startJob(const QUrl &url) override;

private:
    void slotResult(KJob *);
};

#endif // AUTOCONFIGKOLABMAIL_H
