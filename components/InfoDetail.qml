// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: BSD-2-Clause

import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.20 as Kirigami
import org.kde.pim.accountwizard 1.0

ColumnLayout {
    QQC2.ToolButton {
        text: i18n("Details")
        enable: message.text.length > 0
        onClicked: message.visible = !message.visible
    }

    QQC2.Label {
        id: message
        visible: false
        Layout.fillWidth: true
    }

    Connections {
        target: SetupManager
        function onInfoOccured(infoMessage) {
            message.text += infoMessage + '<br />';
        }
    }
}
