/*
    SPDX-FileCopyrightText: 2009 Volker Krause <vkrause@kde.org>
    SPDX-FileCopyrightText: 2023 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "transport.h"
#include <KLocalizedString>
#include <MailTransport/TransportManager>

Transport::Transport(const QString &type, QObject *parent)
    : QObject(parent)
{
}

Transport::~Transport() = default;

void Transport::createTransport()
{
    // TODO
}
