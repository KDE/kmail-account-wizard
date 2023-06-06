/*
    SPDX-FileCopyrightText: 2009 Volker Krause <vkrause@kde.org>
    SPDX-FileCopyrightText: 2023 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "setupbase.h"

#include <QObject>

class Transport : public SetupBase
{
    Q_OBJECT
public:
    struct TransportInfo {
        QString name;
        QString host;
        QString user;
        QString password;
        QString encrStr;
        QString authStr;
        int port = -1;
    };
    explicit Transport(const QString &type, QObject *parent);
    ~Transport() override;
    void createTransport();

    Q_REQUIRED_RESULT TransportInfo transportInfo() const;
    void setTransportInfo(const TransportInfo &newTransportInfo);

Q_SIGNALS:
    void error(const QString &msg);
    void info(const QString &msg);
    void finished(const QString &msg);

private:
    TransportInfo mTransportInfo;
};
QDebug operator<<(QDebug d, const Transport::TransportInfo &t);
