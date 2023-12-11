/*
    SPDX-FileCopyrightText: 2009 Volker Krause <vkrause@kde.org>
    SPDX-FileCopyrightText: 2023 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once
#include "libaccountwizard_private_export.h"
#include "setupbase.h"

#include <QObject>

class LIBACCOUNTWIZARD_TESTS_EXPORT Transport : public SetupBase
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

    [[nodiscard]] TransportInfo transportInfo() const;
    void setTransportInfo(const TransportInfo &newTransportInfo);

private:
    TransportInfo mTransportInfo;
};
QDebug operator<<(QDebug d, const Transport::TransportInfo &t);
Q_DECLARE_TYPEINFO(Transport::TransportInfo, Q_RELOCATABLE_TYPE);
