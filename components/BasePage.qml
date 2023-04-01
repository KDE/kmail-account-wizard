// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: BSD-2-Clause

import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC2
import org.kde.kirigami 2.20 as Kirigami
import org.kde.pim.accountwizard 1.0

Kirigami.ScrollablePage {
    header: QQC2.Control {
        leftPadding: Kirigami.Units.smallSpacing
        rightPadding: Kirigami.Units.smallSpacing
        topPadding: Kirigami.Units.smallSpacing
        bottomPadding: Kirigami.Units.smallSpacing

        contentItem: Kirigami.InlineMessage {
            id: message
            visible: false
        }

        Connections {
            target: SetupManager
            function onErrorOccured(errorMessage) {
                message.text = errorMessage;
                message.visible = true;
            }
        }
    }
}
