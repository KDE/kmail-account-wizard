/*
    SPDX-FileCopyrightText: 2009 Volker Krause <vkrause@kde.org>
    SPDX-FileCopyrightText: 2023 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QObject>

class Transport : public QObject
{
    Q_OBJECT
public:
    explicit Transport(const QString &type, QObject *parent);
    ~Transport() override;
    void createTransport();
};
