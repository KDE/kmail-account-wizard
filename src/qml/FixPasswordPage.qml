// SPDX-FileCopyrightText: 2026 Alan Thouvenin <ath@enioka.com>
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

    property bool loading: false;
    property int index;
    property bool calendarChecked;

    title: i18n("Fix Password")

    LoadingOverlay {
        parent: applicationWindow().contentItem
        loading: root.loading
    }

    nextAction {
        enabled: passwordField.text.length > 0
        onTriggered: {
            root.loading = true;
            SetupManager.password = passwordField.text;
            SetupManager.configurationModel.testLogin(configurationGroup.checkedButton.index);
        }
    }

    Connections {
        target: SetupManager.configurationModel

        function onLoginTestFinished(success) {
            if (!root.loading) {
                return;
                // Then the signal wasn't meant to be handled by this page
            }

            if (success) {
                applicationWindow().pageStack.push(Qt.createComponent('org.kde.pim.accountwizard', 'DetailsPage'));
                SetupManager.configurationModel.createAutomaticAccount(root.index, ConsoleLog, root.calendarChecked);
            }
            root.loading = false;
        }
    }

    header: Component.Banner {
        id: header
        width: parent.width
        type: Kirigami.MessageType.Error
        text: i18n("Login failed, please try correcting your password")
        visible: true
    }

    FormCard.FormHeader {
        title: i18n("Personal Information")
    }

    ColumnLayout {
        Layout.maximumWidth: Kirigami.Units.gridUnit * 30
        Layout.alignment: Qt.AlignHCenter

        FormCard.FormCard {
            FormCard.FormTextFieldDelegate {
                id: nameField
                label: i18n("Full name:")
                placeholderText: i18nc("Generic name", "John Smith")
                text: SetupManager.fullName
                enabled: false
            }

            FormCard.FormDelegateSeparator {}

            FormCard.FormTextFieldDelegate {
                id: addressEmailField
                label: i18n("E-mail address:")
                text: SetupManager.email
                enabled: false
            }

            FormCard.FormDelegateSeparator {}

            FormCard.FormPasswordFieldDelegate {
                id: passwordField
                label: i18n("Password:")
                text: SetupManager.password
                onTextChanged: SetupManager.password = text
                onAccepted: nextAction.triggered()
            }
        }

        QQC2.Button {
            Layout.topMargin: Kirigami.Units.largeSpacing * 2

            action: Kirigami.Action {
                text: i18nc("@action:button", i18n("Edit configuration..."))
                onTriggered: applicationWindow().pageStack.push(Qt.createComponent('org.kde.pim.accountwizard', 'ManualConfigurationPage'), {index: root.index})
            }
        }
    }
}
