// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
// SPDX-FileCopyrightText: 2023-2025 Laurent Montel <montel@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2
import org.kde.kirigami as Kirigami
import org.kde.pim.accountwizard as Account
import org.kde.kirigamiaddons.formcard as FormCard

WizardPage {
    id: root

    title: i18nc("@title:group", "Details")

    nextAction {
        text: i18nc("@action:button", "Quit")
        icon.name: 'application-exit-symbolic'
        onTriggered: Qt.quit();
    }

    FormCard.FormHeader {
        title: i18n("Details")
    }

    FormCard.FormCard {
        id: details

        Repeater {
            id: repeater

            model: Account.ConsoleLog

            delegate: ColumnLayout {
                id: logDelegate

                required property int index
                required property string output
                required property int type

                width: parent.width

                FormCard.FormDelegateSeparator {
                    visible: index !== 0
                    opacity: 1
                }

                FormCard.FormTextDelegate {
                    id: detailsInfo
                    text: logDelegate.output
                    textItem.wrapMode: Text.WordWrap
                }
            }
        }

        FormCard.FormTextDelegate {
            id: placeholder

            visible: repeater.count === 0
            text: i18nc("Placeholder", "No details available.")
            textItem.wrapMode: Text.WordWrap
        }
    }
}
