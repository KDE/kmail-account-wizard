// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
// SPDX-FileCopyrightText: 2023 Laurent Montel <montel@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15 as QQC2
import org.kde.kirigami 2.15 as Kirigami
import org.kde.pim.accountwizard 1.0
import org.kde.kirigamiaddons.labs.mobileform 0.1 as MobileForm

Kirigami.ScrollablePage {
    id: root
    title: i18n("Personal Information")

    leftPadding: 20

    function isNotEmptyStr(str) {
        return str.trim().length > 0;
    }


    ColumnLayout {
        MobileForm.FormCard {
            Layout.topMargin: Kirigami.Units.largeSpacing
            Layout.fillWidth: true
            contentItem: ColumnLayout {
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

                MobileForm.FormTextFieldDelegate {
                    label: i18n("Full name:")
                    placeholderText: i18nc("Generic name", "John Smith")
                    text: SetupManager.name
                    onTextChanged: SetupManager.name = text
                }

                MobileForm.FormDelegateSeparator {}

                MobileForm.FormTextFieldDelegate {
                    id: addressEmailField
                    label: i18n("E-mail address:")
                    placeholderText: i18nc("Generic email address", "boss@example.corp")
                    text: SetupManager.email
                    onTextChanged: {
                        SetupManager.email = text
                    }
                }

                MobileForm.FormDelegateSeparator {}

                MobileForm.FormTextFieldDelegate {
                    label: i18n("Password:")
                    onTextChanged: SetupManager.password = text
                    echoMode: TextInput.Password
                    inputMethodHints: Qt.ImhUrlCharactersOnly
                }

                MobileForm.FormDelegateSeparator { above: continueButton }

                MobileForm.FormButtonDelegate {
                    id: continueButton
                    text: i18n("Continue")
                    checked: true
                    onClicked: SetupManager.searchConfiguration()
                    enabled: isNotEmptyStr(addressEmailField.text) // Fix trimmed + is real email
                }

                MobileForm.FormDelegateSeparator { below: continueButton }

                MobileForm.FormSectionText {
                    text: i18n("Check online for the settings needed for this email provider. Only the domain name part of the e-mail address will be sent over the Internet.")
                    wrapMode: Text.WordWrap
                }
            }
        }

        MobileForm.FormCard {
            id: ispdbSearchInfo
            Layout.topMargin: Kirigami.Units.largeSpacing
            Layout.fillWidth: true
            visible: SetupManager.searchIspdbFoundMessage.length > 0
            contentItem: ColumnLayout {
                spacing: 0

                Kirigami.InlineMessage {
                    Layout.fillWidth: true
                    type: Kirigami.MessageType.Information
                    text: SetupManager.searchIspdbFoundMessage
                    visible: ispdbSearchInfo.visible
                }
            }
        }

        MobileForm.FormCard {
            id: noIspdbConfigFound
            Layout.topMargin: Kirigami.Units.largeSpacing
            Layout.fillWidth: true
            visible: SetupManager.noConfigFound
            contentItem: ColumnLayout {
                spacing: 0

                Kirigami.InlineMessage {
                    Layout.fillWidth: true
                    type: Kirigami.MessageType.Error
                    text: i18n("No config found from ispdb server.")
                    visible: noIspdbConfigFound.visible
                }
            }
        }

        MobileForm.FormCard {
            id: manualConfiguration
            Layout.topMargin: Kirigami.Units.largeSpacing
            Layout.fillWidth: true
            visible: SetupManager.noConfigFound
            contentItem: ColumnLayout {
                spacing: 0

                MobileForm.FormCardHeader {
                    title: i18n("Server Parameters")
                }

                QQC2.Label {
                    font.bold: true
                    text: i18n("Incoming")
                    padding: Kirigami.Units.gridUnit
                }

                MobileForm.FormTextFieldDelegate {
                    id: manualIncomingHostName
                    label: i18n("Hostname:")
                    inputMethodHints: Qt.ImhUrlCharactersOnly
                    text: SetupManager.manualConfiguration.incomingHostName
                }
                MobileForm.FormComboBoxDelegate {
                    id: manualIncomingProtocol
                    description: i18n("Protocol:")
                    model: SetupManager.manualConfiguration.incomingProtocols
                    currentIndex: SetupManager.manualConfiguration.currentIncomingProtocol
                    onCurrentIndexChanged: {
                        SetupManager.manualConfiguration.currentIncomingProtocol = currentIndex
                    }
                }
                MobileForm.FormSpinBoxDelegate {
                    id: manualIncomingPort
                    label: i18n("Port:")
                    value: SetupManager.manualConfiguration.incomingPort
                    from: 1
                    to: 999
                }
                MobileForm.FormComboBoxDelegate {
                    id: manualIncomingSecurity
                    description: i18n("Security:")
                    model: SetupManager.manualConfiguration.securityProtocols
                    currentIndex: SetupManager.manualConfiguration.currentIncomingAuthenticationProtocols
                    onCurrentIndexChanged: {
                        SetupManager.manualConfiguration.currentIncomingAuthenticationProtocols = currentIndex
                    }
                }
                MobileForm.FormComboBoxDelegate {
                    id: manualIncomingAuthenticationMethod
                    description: i18n("Authentication Method:")
                    model: SetupManager.manualConfiguration.authenticationProtocols
                    currentIndex: SetupManager.manualConfiguration.currentOutgoingSecurityProtocol
                    onCurrentIndexChanged: {
                        SetupManager.manualConfiguration.currentOutgoingSecurityProtocol = currentIndex
                    }
                }
                MobileForm.FormTextFieldDelegate {
                    id: manualIncomingUserName
                    label: i18n("Username:")
                    inputMethodHints: Qt.ImhUrlCharactersOnly
                    text: SetupManager.manualConfiguration.userName
                }

                MobileForm.FormDelegateSeparator {}

                QQC2.Label {
                    font.bold: true
                    text: i18n("Outgoing")
                    padding: Kirigami.Units.gridUnit
                }
                MobileForm.FormTextFieldDelegate {
                    id: manualOutgoingHostName
                    label: i18n("Hostname:")
                    inputMethodHints: Qt.ImhUrlCharactersOnly
                    text: SetupManager.manualConfiguration.manualOutgoingHostName
                }
                MobileForm.FormSpinBoxDelegate {
                    id: manualOutgoingPort
                    label: i18n("Port:")
                    value: SetupManager.manualConfiguration.outgoingPort
                    from: 1
                    to: 999
                }
                MobileForm.FormComboBoxDelegate {
                    id: manualOutgoingSecurity
                    description: i18n("Security:")
                    model: SetupManager.manualConfiguration.securityProtocols
                    currentIndex: SetupManager.manualConfiguration.currentOutgoingSecurityProtocol
                    onCurrentIndexChanged: {
                        SetupManager.manualConfiguration.currentOutgoingSecurityProtocol = currentIndex
                    }
                }
                MobileForm.FormComboBoxDelegate {
                    id: manualOutgoingAuthenticationMethod
                    description: i18n("Authentication Method:")
                    model: SetupManager.manualConfiguration.authenticationProtocols
                    currentIndex: SetupManager.manualConfiguration.currentOutgoingAuthenticationProtocols
                    onCurrentIndexChanged: {
                        SetupManager.manualConfiguration.currentOutgoingAuthenticationProtocols = currentIndex
                    }
                }
                MobileForm.FormTextFieldDelegate {
                    id: manualOutgoingUserName
                    label: i18n("Username:")
                    inputMethodHints: Qt.ImhUrlCharactersOnly
                    text: SetupManager.manualConfiguration.manualOutgoingUserName
                }
                MobileForm.FormDelegateSeparator {}

                MobileForm.FormButtonDelegate {
                    id: recheckAccountManualConfiguration
                    text: i18n("Recheck")
                    checked: true
                    onClicked: {
                        SetupManager.manualConfiguration.checkServer()
                    }
                    visible: manualConfiguration.visible
                    enabled: SetupManager.manualConfiguration.configurationIsValid
                }

                MobileForm.FormDelegateSeparator {}

                MobileForm.FormButtonDelegate {
                    id: createAccountManualConfiguration
                    text: i18n("Create Account")
                    checked: true
                    onClicked: {
                        SetupManager.manualConfiguration.createManualAccount()
                    }
                    visible: manualConfiguration.visible
                    enabled: SetupManager.manualConfiguration.configurationIsValid
                }
            }
        }

        MobileForm.FormCard {
            id: availableConfigurations
            Layout.topMargin: Kirigami.Units.largeSpacing
            Layout.fillWidth: true

            visible: configurationRepeater.count > 0

            contentItem: ColumnLayout {
                spacing: 0

                MobileForm.FormCardHeader {
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

                MobileForm.FormDelegateSeparator {}

                MobileForm.FormButtonDelegate {
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
