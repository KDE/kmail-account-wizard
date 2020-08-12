/*
    SPDX-FileCopyrightText: 2009 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef GLOBAL_H
#define GLOBAL_H

#include <QUrl>
#include <QStringList>

namespace Global {
Q_REQUIRED_RESULT QStringList typeFilter();
void setTypeFilter(const QStringList &filter);
void setPoFileName(const QString &poFileName);
Q_REQUIRED_RESULT QString poFileName();
Q_REQUIRED_RESULT QString assistant();
void setAssistant(const QString &assistant);

Q_REQUIRED_RESULT QString assistantBasePath();
Q_REQUIRED_RESULT QStringList assistants();

Q_REQUIRED_RESULT QString unpackAssistant(const QUrl &remotePackageUrl);
}

#endif
