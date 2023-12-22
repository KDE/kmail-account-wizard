// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
// SPDX-FileCopyrightText: 2023 Laurent Montel <montel@kde.org>
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

    FormCard.FormHeader {
        title: i18n("Set Up Your Existing Email Address")
    }

    FormCard.FormCard {
        Kirigami.Heading {
            text: i18n("To use your current email address fill in your credentials.<br />This wizard will automatically search for a working and recommended server configuration.")
            padding: Kirigami.Units.gridUnit
            bottomPadding: Kirigami.Units.largeSpacing
            topPadding: Kirigami.Units.largeSpacing
            wrapMode: Text.WordWrap
            level: 4

            Layout.fillWidth: true
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormTextFieldDelegate {
            label: i18n("Full name:")
            placeholderText: i18nc("Generic name", "John Smith")
            text: SetupManager.name
            onTextChanged: SetupManager.name = text
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormTextFieldDelegate {
            id: addressEmailField
            label: i18n("E-mail address:")
            placeholderText: i18nc("Generic email address", "boss@example.corp")
            text: SetupManager.email
            onTextChanged: {
                SetupManager.email = text
            }
            onAccepted: {
                SetupManager.searchConfiguration()
            }
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormPasswordFieldDelegate {
            id: passwordField
            onAccepted: {
                SetupManager.password = text
            }
            label: i18n("Password:")
        }

        FormCard.FormDelegateSeparator { above: continueButton }

        FormCard.FormButtonDelegate {
            id: continueButton
            text: i18n("Continue")
            checked: true
            onClicked: SetupManager.searchConfiguration()
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
        id: manualConfiguration
        visible: SetupManager.noConfigFound || root.explicitManualConfiguration
        FormCard.FormTextFieldDelegate {
            id: manualIncomingHostName
            label: i18n("Incoming server:")
            inputMethodHints: Qt.ImhUrlCharactersOnly
            text: SetupManager.manualConfiguration.incomingHostName
            onTextChanged: {
                SetupManager.manualConfiguration.incomingHostName = manualIncomingHostName.text
            }
        }
        FormCard.FormComboBoxDelegate {
            id: manualIncomingProtocol
            description: i18n("Protocol:")
            model: SetupManager.manualConfiguration.incomingProtocols
            currentIndex: SetupManager.manualConfiguration.currentIncomingProtocol
            onCurrentIndexChanged: {
                SetupManager.manualConfiguration.currentIncomingProtocol = currentIndex
            }
        }
        FormCard.FormSpinBoxDelegate {
            id: manualIncomingPort
            label: i18n("Port:")
            value: SetupManager.manualConfiguration.incomingPort
            from: 1
            to: 999
        }
        FormCard.FormComboBoxDelegate {
            id: manualIncomingSecurity
            description: i18n("Security:")
            model: SetupManager.manualConfiguration.securityProtocols
            currentIndex: SetupManager.manualConfiguration.currentIncomingSecurityProtocol
            onCurrentIndexChanged: {
                SetupManager.manualConfiguration.currentIncomingSecurityProtocol = currentIndex
            }
        }
        FormCard.FormComboBoxDelegate {
            id: manualIncomingAuthenticationMethod
            description: i18n("Authentication Method:")
            model: SetupManager.manualConfiguration.authenticationProtocols
            currentIndex: SetupManager.manualConfiguration.currentIncomingAuthenticationProtocol
            onCurrentIndexChanged: {
                SetupManager.manualConfiguration.currentIncomingAuthenticationProtocols = currentIndex
            }
        }
        FormCard.FormTextFieldDelegate {
            id: manualIncomingUserName
            label: i18n("Username:")
            inputMethodHints: Qt.ImhUrlCharactersOnly
            text: SetupManager.manualConfiguration.incomingUserName
            onTextChanged: {
                SetupManager.manualConfiguration.incomingUserName = manualIncomingUserName.text
            }
        }

        FormCard.FormCheckDelegate {
            id: disconnectedModeEnabled
            visible: SetupManager.manualConfiguration.hasDisconnectedMode
            description: i18n("Download all messages for offline use")
            onCheckedChanged: (checked) => {
                SetupManager.manualConfiguration.disconnectedModeEnabled = checked
            }
            checked: SetupManager.manualConfiguration.disconnectedModeEnabled
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
            text: SetupManager.manualConfiguration.outgoingHostName
            onTextChanged: {
                SetupManager.manualConfiguration.outgoingHostName = manualOutgoingHostName.text
            }
        }
        FormCard.FormSpinBoxDelegate {
            id: manualOutgoingPort
            label: i18n("Port:")
            value: SetupManager.manualConfiguration.outgoingPort
            from: 1
            to: 999
        }
        FormCard.FormComboBoxDelegate {
            id: manualOutgoingSecurity
            description: i18n("Security:")
            model: SetupManager.manualConfiguration.securityProtocols
            currentIndex: SetupManager.manualConfiguration.currentOutgoingSecurityProtocol
            onCurrentIndexChanged: {
                SetupManager.manualConfiguration.currentOutgoingSecurityProtocol = currentIndex
            }
        }
        FormCard.FormComboBoxDelegate {
            id: manualOutgoingAuthenticationMethod
            description: i18n("Authentication Method:")
            model: SetupManager.manualConfiguration.authenticationProtocols
            currentIndex: SetupManager.manualConfiguration.currentOutgoingAuthenticationProtocol
            onCurrentIndexChanged: {
                SetupManager.manualConfiguration.currentOutgoingAuthenticationProtocols = currentIndex
            }
        }
        FormCard.FormTextFieldDelegate {
            id: manualOutgoingUserName
            label: i18n("Username:")
            inputMethodHints: Qt.ImhUrlCharactersOnly
            text: SetupManager.manualConfiguration.outgoingUserName
            onTextChanged: {
                SetupManager.manualConfiguration.outgoingUserName = manualOutgoingUserName.text
            }
        }
        FormCard.FormDelegateSeparator {}

        FormCard.FormButtonDelegate {
            id: recheckAccountManualConfiguration
            text: i18n("Recheck")
            checked: true
            onClicked: {
                SetupManager.manualConfiguration.checkServer()
            }
            visible: manualConfiguration.visible
            enabled: SetupManager.manualConfiguration.configurationIsValid
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormButtonDelegate {
            id: createAccountManualConfiguration
            text: i18n("Create Account")
            checked: true
            onClicked: {
                SetupManager.createManualAccount()
            }
            visible: manualConfiguration.visible
            enabled: SetupManager.manualConfiguration.configurationIsValid
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
            // Component.onCompleted: console.log(SetupManager.configurationModel)

            delegate: ConfigurationDelegate {
                required property int index

                checked: index === 0
                Layout.fillWidth: true
            }
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormButtonDelegate {
            id: configureManual
            text: i18n("Manual Configuration")
            checked: true
            onClicked: {
                root.explicitManualConfiguration = true;
            }
            visible: availableConfigurations.visible
        }

        FormCard.FormButtonDelegate {
            id: createAccount
            text: i18n("Create Account")
            checked: true
            onClicked: SetupManager.createAutomaticAccount()
            visible: availableConfigurations.visible
        }
    }

    FormCard.FormHeader {
        visible: SetupManager.details.length > 0
        title: i18n("Details")
    }

    FormCard.FormCard {
        id: details
        visible: SetupManager.details.length > 0
        FormCard.FormTextDelegate {
            id: detailsInfo
            text: SetupManager.details
        }
    }
}
