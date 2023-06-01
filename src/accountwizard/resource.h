/*
    SPDX-FileCopyrightText: 2023 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QObject>

class Resource : public QObject
{
    Q_OBJECT
public:
    explicit Resource(const QString &resourceType, QObject *parent = nullptr);
    ~Resource() override;

private:
    const QString mTypeIdentifier;
};
