// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
// SPDX-FileCopyrightText: 2023 Laurent Montel <montel@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15 as QQC2
import org.kde.kirigami 2.15 as Kirigami
import org.kde.pim.accountwizard 1.0
import org.kde.kirigamiaddons.formcard 1 as FormCard

Kirigami.ScrollablePage {
    id: root
    title: i18n("Personal Information")

    leftPadding: 20

    function isNotEmptyStr(str) {
        return str.trim().length > 0;
    }


    ColumnLayout {
        FormCard.FormCard {
            Layout.topMargin: Kirigami.Units.largeSpacing
            Layout.fillWidth: true
            ColumnLayout {
                spacing: 0

                Kirigami.Heading {
                    text: i18n("Set Up Your Existing Email Address")
                    wrapMode: Text.WordWrap
                    padding: Kirigami.Units.gridUnit
                    bottomPadding: 0

                    Layout.fillWidth: true
                }

                Kirigami.Heading {
                    text: i18n("To use your current email address fill in your credentials.<br />This wizard will automatically search for a working and recommended server configuration.")
                    padding: Kirigami.Units.gridUnit
                    topPadding: 0
                    bottomPadding: 0
                    wrapMode: Text.WordWrap
                    level: 4

                    Layout.fillWidth: true
                }

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
                    onEditingFinished: {
                        SetupManager.searchConfiguration()
                    }
                }

                FormCard.FormDelegateSeparator {}

                FormCard.FormTextFieldDelegate {
                    label: i18n("Password:")
                    onTextChanged: SetupManager.password = text
                    echoMode: TextInput.Password
                    inputMethodHints: Qt.ImhUrlCharactersOnly
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
        }

        FormCard.FormCard {
            id: ispdbSearchInfo
            Layout.topMargin: Kirigami.Units.largeSpacing
            Layout.fillWidth: true
            visible: SetupManager.searchIspdbFoundMessage.length > 0
            ColumnLayout {
                spacing: 0

                Kirigami.InlineMessage {
                    Layout.fillWidth: true
                    type: Kirigami.MessageType.Information
                    text: SetupManager.searchIspdbFoundMessage
                    visible: ispdbSearchInfo.visible
                }
            }
        }

        FormCard.FormCard {
            id: noIspdbConfigFound
            Layout.topMargin: Kirigami.Units.largeSpacing
            Layout.fillWidth: true
            visible: SetupManager.noConfigFound
            ColumnLayout {
                spacing: 0

                Kirigami.InlineMessage {
                    Layout.fillWidth: true
                    type: Kirigami.MessageType.Error
                    text: i18n("No config found from ispdb server.")
                    visible: noIspdbConfigFound.visible
                }
            }
        }

        FormCard.FormCard {
            id: manualConfiguration
            Layout.topMargin: Kirigami.Units.largeSpacing
            Layout.fillWidth: true
            visible: SetupManager.noConfigFound
            ColumnLayout {
                spacing: 0

                FormCard.FormHeader {
                    title: i18n("Server Parameters")
                }

                QQC2.Label {
                    font.bold: true
                    text: i18n("Incoming")
                    padding: Kirigami.Units.gridUnit
                }

                FormCard.FormTextFieldDelegate {
                    id: manualIncomingHostName
                    label: i18n("Hostname:")
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
                    currentIndex: SetupManager.manualConfiguration.currentIncomingAuthenticationProtocols
                    onCurrentIndexChanged: {
                        SetupManager.manualConfiguration.currentIncomingAuthenticationProtocols = currentIndex
                    }
                }
                FormCard.FormComboBoxDelegate {
                    id: manualIncomingAuthenticationMethod
                    description: i18n("Authentication Method:")
                    model: SetupManager.manualConfiguration.authenticationProtocols
                    currentIndex: SetupManager.manualConfiguration.currentOutgoingSecurityProtocol
                    onCurrentIndexChanged: {
                        SetupManager.manualConfiguration.currentOutgoingSecurityProtocol = currentIndex
                    }
                }
                FormCard.FormTextFieldDelegate {
                    id: manualIncomingUserName
                    label: i18n("Username:")
                    inputMethodHints: Qt.ImhUrlCharactersOnly
                    text: SetupManager.manualConfiguration.userName
                    onTextChanged: {
                        SetupManager.manualConfiguration.userName = manualIncomingUserName.text
                    }
                }

                FormCard.FormDelegateSeparator {}

                QQC2.Label {
                    font.bold: true
                    text: i18n("Outgoing")
                    padding: Kirigami.Units.gridUnit
                }
                FormCard.FormTextFieldDelegate {
                    id: manualOutgoingHostName
                    label: i18n("Hostname:")
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
                    currentIndex: SetupManager.manualConfiguration.currentOutgoingAuthenticationProtocols
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
        }

        FormCard.FormCard {
            id: availableConfigurations
            Layout.topMargin: Kirigami.Units.largeSpacing
            Layout.fillWidth: true

            visible: configurationRepeater.count > 0

            ColumnLayout {
                spacing: 0

                FormCard.FormHeader {
                    title: i18n("Available configurations")
                }

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
                    id: createAccount
                    text: i18n("Create Account")
                    checked: true
                    onClicked: SetupManager.createAutomaticAccount()
                    visible: availableConfigurations.visible
                }
            }
        }
    }
}
