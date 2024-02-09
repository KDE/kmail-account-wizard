// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
// SPDX-FileCopyrightText: 2023-2024 Laurent Montel <montel.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2
import org.kde.kirigami as Kirigami
import org.kde.pim.accountwizard
import org.kde.kirigamiaddons.formcard as FormCard

FormCard.FormCardPage {
    id: root

    title: i18n("Personal Information")

    property bool explicitManualConfiguration: false

    function isNotEmptyStr(str) {
        return str.trim().length > 0;
    }

    readonly property Connections manuConfigurationConnections: Connections {
        target: SetupManager.accountConfiguration

        function onIncomingHostNameChanged(): void {
            manualIncomingHostName.text = SetupManager.accountConfiguration.incomingHostName;
        }

        function onOutgoingHostNameChanged(): void {
            manualOutgoingHostName.text = SetupManager.accountConfiguration.outgoingHostName;
        }

        function onIncomingUserNameChanged(): void {
            manualIncomingUserName.text = SetupManager.accountConfiguration.incomingUserName;
        }
        function onOutgoingUserNameChanged(): void {
            manualOutgoingUserName.text = SetupManager.accountConfiguration.outgoingUserName;
        }
    }

    FormCard.FormHeader {
        title: i18n("Set Up Your Existing Email Address")
    }

    FormCard.FormCard {
        QQC2.Label {
            text: i18n("To use your current email address fill in your credentials.<br />This wizard will automatically search for a working and recommended server configuration.")
            padding: Kirigami.Units.gridUnit
            bottomPadding: Kirigami.Units.largeSpacing
            topPadding: Kirigami.Units.largeSpacing
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormTextFieldDelegate {
            id: nameField
            label: i18n("Full name:")
            placeholderText: i18nc("Generic name", "John Smith")
            text: SetupManager.name
            onTextEdited: SetupManager.name = text
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormTextFieldDelegate {
            id: addressEmailField
            label: i18n("E-mail address:")
            placeholderText: i18nc("Generic email address", "boss@example.corp")
            text: SetupManager.email
            onTextEdited: {
                SetupManager.email = text
                root.explicitManualConfiguration = false;
            }
            onAccepted: continueButton.clicked()
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormPasswordFieldDelegate {
            id: passwordField
            label: i18n("Password:")
            onTextEdited: SetupManager.password = text
            onAccepted: continueButton.clicked()
        }

        FormCard.FormDelegateSeparator { above: continueButton }

        FormCard.FormButtonDelegate {
            id: continueButton
            text: i18n("Continue")
            checked: true
            onClicked: {
                SetupManager.name = nameField.text;
                SetupManager.email = addressEmailField.text;
                SetupManager.password = passwordField.text;
                SetupManager.searchConfiguration()
            }
            enabled: isNotEmptyStr(addressEmailField.text) // Fix trimmed + is real email
        }

        FormCard.FormDelegateSeparator { below: continueButton }

        FormCard.FormSectionText {
            text: i18n("Check online for the settings needed for this email provider. Only the domain name part of the e-mail address will be sent over the Internet.")
            wrapMode: Text.WordWrap
        }
    }

    FormCard.FormCard {
        id: ispdbSearchInfo
        Layout.topMargin: Kirigami.Units.largeSpacing
        Layout.fillWidth: true
        visible: SetupManager.searchIspdbFoundMessage.length > 0
        Kirigami.InlineMessage {
            Layout.fillWidth: true
            type: Kirigami.MessageType.Information
            text: SetupManager.searchIspdbFoundMessage
            visible: ispdbSearchInfo.visible
        }
    }

    FormCard.FormCard {
        id: noIspdbConfigFound
        Layout.topMargin: Kirigami.Units.largeSpacing
        Layout.fillWidth: true
        visible: SetupManager.noConfigFound
        Kirigami.InlineMessage {
            Layout.fillWidth: true
            type: Kirigami.MessageType.Error
            text: i18n("No config found from ispdb server.")
            visible: noIspdbConfigFound.visible
        }
    }

    FormCard.FormHeader {
        visible: SetupManager.noConfigFound || root.explicitManualConfiguration
        title: i18n("Incoming Server Parameters")
    }

    FormCard.FormCard {
        id: accountConfiguration
        visible: SetupManager.noConfigFound || root.explicitManualConfiguration
        FormCard.FormTextFieldDelegate {
            id: manualIncomingHostName
            label: i18n("Incoming server:")
            inputMethodHints: Qt.ImhUrlCharactersOnly
            text: SetupManager.accountConfiguration.incomingHostName;
            onTextEdited: {
                SetupManager.accountConfiguration.incomingHostName = manualIncomingHostName.text
            }
        }

        FormCard.FormComboBoxDelegate {
            id: manualIncomingProtocol
            text: i18n("Protocol:")
            model: SetupManager.accountConfiguration.incomingProtocols
            currentIndex: SetupManager.accountConfiguration.currentIncomingProtocol
            onCurrentIndexChanged: {
                SetupManager.accountConfiguration.currentIncomingProtocol = currentIndex
            }
        }
        FormCard.FormSpinBoxDelegate {
            id: manualIncomingPort
            label: i18n("Port:")
            value: SetupManager.accountConfiguration.incomingPort
            from: 1
            to: 999
        }
        FormCard.FormComboBoxDelegate {
            id: manualIncomingSecurity
            text: i18n("Security:")
            model: SetupManager.accountConfiguration.securityProtocols
            currentIndex: SetupManager.accountConfiguration.currentIncomingSecurityProtocol
            onCurrentIndexChanged: {
                SetupManager.accountConfiguration.currentIncomingSecurityProtocol = currentIndex
            }
        }
        FormCard.FormComboBoxDelegate {
            id: manualIncomingAuthenticationMethod
            text: i18n("Authentication Method:")
            model: SetupManager.accountConfiguration.authenticationProtocols
            currentIndex: SetupManager.accountConfiguration.currentIncomingAuthenticationProtocol
            onCurrentIndexChanged: {
                SetupManager.accountConfiguration.currentIncomingAuthenticationProtocols = currentIndex
            }
        }
        FormCard.FormTextFieldDelegate {
            id: manualIncomingUserName
            label: i18n("Username:")
            inputMethodHints: Qt.ImhUrlCharactersOnly
            text: SetupManager.accountConfiguration.incomingUserName
            onTextEdited: {
                SetupManager.accountConfiguration.incomingUserName = manualIncomingUserName.text
            }
        }

        FormCard.FormCheckDelegate {
            id: disconnectedModeEnabled
            visible: SetupManager.accountConfiguration.hasDisconnectedMode
            description: i18n("Download all messages for offline use")
            onCheckedChanged: {
                SetupManager.accountConfiguration.disconnectedModeEnabled = checked
            }
            checked: SetupManager.accountConfiguration.disconnectedModeEnabled
        }
    }

    FormCard.FormHeader {
        visible: SetupManager.noConfigFound || root.explicitManualConfiguration
        title: i18n("Outgoing Server Parameters")
    }

    FormCard.FormCard {
        visible: SetupManager.noConfigFound || root.explicitManualConfiguration
        FormCard.FormTextFieldDelegate {
            id: manualOutgoingHostName
            label: i18n("Outgoing server:")
            inputMethodHints: Qt.ImhUrlCharactersOnly
            text: SetupManager.accountConfiguration.outgoingHostName
            onTextEdited: {
                SetupManager.accountConfiguration.outgoingHostName = manualOutgoingHostName.text
            }
        }
        FormCard.FormSpinBoxDelegate {
            id: manualOutgoingPort
            label: i18n("Port:")
            value: SetupManager.accountConfiguration.outgoingPort
            from: 1
            to: 999
        }
        FormCard.FormComboBoxDelegate {
            id: manualOutgoingSecurity
            text: i18n("Security:")
            model: SetupManager.accountConfiguration.securityProtocols
            currentIndex: SetupManager.accountConfiguration.currentOutgoingSecurityProtocol
            onCurrentIndexChanged: {
                SetupManager.accountConfiguration.currentOutgoingSecurityProtocol = currentIndex
            }
        }
        FormCard.FormComboBoxDelegate {
            id: manualOutgoingAuthenticationMethod
            text: i18n("Authentication Method:")
            model: SetupManager.accountConfiguration.authenticationProtocols
            currentIndex: SetupManager.accountConfiguration.currentOutgoingAuthenticationProtocol
            onCurrentIndexChanged: {
                SetupManager.accountConfiguration.currentOutgoingAuthenticationProtocols = currentIndex
            }
        }
        FormCard.FormTextFieldDelegate {
            id: manualOutgoingUserName
            label: i18n("Username:")
            inputMethodHints: Qt.ImhUrlCharactersOnly
            text: SetupManager.accountConfiguration.outgoingUserName
            onTextEdited: {
                SetupManager.accountConfiguration.outgoingUserName = manualOutgoingUserName.text
            }
        }
    }

    FormCard.FormCard {
        Layout.topMargin: Kirigami.Units.largeSpacing
        visible: SetupManager.noConfigFound || root.explicitManualConfiguration

        FormCard.FormButtonDelegate {
            id: recheckAccountManualConfiguration
            text: i18n("Recheck")
            checked: true
            onClicked: {
                SetupManager.accountConfiguration.checkServer()
            }
            visible: accountConfiguration.visible
            enabled: SetupManager.accountConfiguration.configurationIsValid && !SetupManager.accountConfiguration.serverTestInProgress
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormButtonDelegate {
            id: createAccountManualConfiguration
            text: i18n("Create Account")
            checked: true
            onClicked: {
                SetupManager.createManualAccount()
            }
            visible: accountConfiguration.visible
            enabled: SetupManager.accountConfiguration.configurationIsValid
        }
    }

    FormCard.FormHeader {
        title: i18n("Available configurations")
        visible: configurationRepeater.count > 0 && !root.explicitManualConfiguration
    }

    FormCard.FormCard {
        id: availableConfigurations

        visible: configurationRepeater.count > 0 && !root.explicitManualConfiguration

        Repeater {
            id: configurationRepeater
            model: SetupManager.configurationModel

            delegate: ConfigurationDelegate {
                required property int index

                checked: index === 0
                Layout.fillWidth: true
            }
        }
    }

    FormCard.FormCard {
        Layout.topMargin: Kirigami.Units.largeSpacing
        visible: configurationRepeater.count > 0 && !root.explicitManualConfiguration

        FormCard.FormButtonDelegate {
            id: configureManual
            text: i18n("Manual Configuration")
            checked: true
            onClicked: {
                root.explicitManualConfiguration = true;
            }
        }

        FormCard.FormButtonDelegate {
            id: createAccount
            text: i18n("Create Account")
            checked: true
            onClicked: SetupManager.createAutomaticAccount()
        }
    }

    FormCard.FormHeader {
        visible: SetupManager.details.length > 0
        title: i18n("Details")
    }

    FormCard.FormCard {
        id: details
        visible: SetupManager.details.length > 0
        QQC2.Label {
            id: detailsInfo
            text: SetupManager.details
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
            padding: Kirigami.Units.gridUnit
            bottomPadding: Kirigami.Units.largeSpacing
            topPadding: Kirigami.Units.largeSpacing
        }
    }
}
