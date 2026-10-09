// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
// SPDX-FileCopyrightText: 2023-2026 Laurent Montel <montel@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick
import org.kde.ki18n
import QtQuick.Layouts
import QtQuick.Controls as QQC2
import org.kde.kirigami as Kirigami
import org.kde.pim.accountwizard as Account
import org.kde.kirigamiaddons.formcard as FormCard

WizardPage {
    id: root

    title: KI18n.i18nc("@title:group", "Details")

    nextAction {
        text: KI18n.i18nc("@action:button", "Finish")
        icon.name: 'dialog-ok'
        onTriggered: Qt.quit();
    }

    FormCard.FormHeader {
        title: KI18n.i18n("Details")
    }

    FormCard.FormCard {
        id: details

        Repeater {
            id: repeater

            model: Account.ConsoleLog

            delegate: FormCard.FormTextDelegate {
                id: logDelegate

                required property int index
                required property string output
                required property int type

                text: logDelegate.output
                textItem.wrapMode: Text.WordWrap
            }
        }

        FormCard.FormTextDelegate {
            id: placeholder

            visible: repeater.count === 0
            text: KI18n.i18nc("Placeholder", "No details available.")
            textItem.wrapMode: Text.WordWrap
        }
    }
}
