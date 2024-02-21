// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
// SPDX-FileCopyrightText: 2023-2024 Laurent Montel <montel.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2
import org.kde.kirigami as Kirigami
import org.kde.pim.accountwizard
import org.kde.kirigamiaddons.formcard as FormCard

WizardPage {
    id: root

    title: i18n("Personal Information")

    function isNotEmptyStr(str) {
        return str.trim().length > 0;
    }

    nextAction {
        onTriggered: {
            SetupManager.fullName = nameField.text;
            SetupManager.email = addressEmailField.text;
            SetupManager.password = passwordField.text;
            SetupManager.searchConfiguration()

            applicationWindow().pageStack.push(Qt.createComponent('org.kde.pim.accountwizard', 'ConfigurationSelectionPage'))
        }
        enabled: isNotEmptyStr(addressEmailField.text) // Fix trimmed + is real email
    }

    Kirigami.Icon {
        source: "kmail"
        implicitWidth: Math.round(Kirigami.Units.iconSizes.huge * 1.5)
        implicitHeight: Math.round(Kirigami.Units.iconSizes.huge * 1.5)

        Layout.alignment: Qt.AlignHCenter
        Layout.topMargin: Kirigami.Units.gridUnit
    }

    Kirigami.Heading {
        text: i18n("Connect your Email Account")

        Layout.alignment: Qt.AlignHCenter
        Layout.topMargin: Kirigami.Units.largeSpacing
        Layout.bottomMargin: Kirigami.Units.largeSpacing
    }

    FormCard.FormSectionText {
        text: i18n("To use your current email address fill in your credentials. This wizard will automatically search for a working and recommended server configuration. Only the domain name part of the e-mail address will be sent over the Internet.")
    }

    FormCard.FormCard {
        FormCard.FormTextFieldDelegate {
            id: nameField
            label: i18n("Full name:")
            placeholderText: i18nc("Generic name", "John Smith")
            text: SetupManager.fullName
            onTextEdited: SetupManager.fullName = text
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormTextFieldDelegate {
            id: addressEmailField
            label: i18n("E-mail address:")
            placeholderText: i18nc("Generic email address", "boss@example.corp")
            text: SetupManager.email
            onTextChanged: {
                SetupManager.email = text;
                if (SetupManager.email.endsWith('@gmail.com')) {
                    passwordField.enabled = false;
                    passwordField.text = '';
                } else {
                    passwordField.enabled = true;
                }
            }
            onAccepted: nextAction.triggered()
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormPasswordFieldDelegate {
            id: passwordField
            label: i18n("Password (optional):")
            onTextChanged: SetupManager.password = text
            onAccepted: nextAction.triggered()
        }
    }
}
