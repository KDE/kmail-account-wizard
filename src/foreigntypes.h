// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include <KIMAP/LoginJob>
#include <MailTransport/Transport>

#include <qqml.h>

struct LoginJob {
    Q_GADGET
    QML_FOREIGN(KIMAP::LoginJob)
    QML_ELEMENT
    QML_UNCREATABLE("Enum")
};

struct Transport {
    Q_GADGET
    QML_FOREIGN(MailTransport::Transport)
    QML_ELEMENT
    QML_UNCREATABLE("Enum")
};
