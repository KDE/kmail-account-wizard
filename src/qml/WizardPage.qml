// SPDX-FileCopyrightText: 2024 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard

FormCard.FormCardPage {
    id: root

    readonly property Kirigami.Action previousAction: Kirigami.Action {
        icon.name: 'go-previous-symbolic'
        text: i18nc("@action:button", "Previous")
        visible: applicationWindow().pageStack.depth > 1
        onTriggered: applicationWindow().pageStack.pop();
    }

    readonly property Kirigami.Action nextAction: Kirigami.Action {
        icon.name: 'go-next-symbolic'
        text: i18nc("@action:button", "Next")
    }

    footer: QQC2.ToolBar {
        contentItem: RowLayout {
            spacing: Kirigami.Units.mediumSpacing

            Item {
                Layout.fillWidth: true
            }

            QQC2.Button {
                action: root.previousAction
                visible: root.previousAction.visible
            }

            QQC2.Button {
                action: root.nextAction
            }
        }
    }
}
