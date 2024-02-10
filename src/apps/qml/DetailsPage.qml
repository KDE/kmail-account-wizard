// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
// SPDX-FileCopyrightText: 2023-2024 Laurent Montel <montel.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2
import org.kde.kirigami as Kirigami
import org.kde.pim.accountwizard
import org.kde.kirigamiaddons.formcard as FormCard
import org.kde.kirigamiaddons.components as Component

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

        FormCard.FormTextDelegate {
            id: detailsInfo
            text: SetupManager.details.length > 0 ? SetupManager.details : i18nc("Placeholder", "No details available.")
            textItem.wrapMode: Text.WordWrap
        }
    }
}
