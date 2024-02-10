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

    title: i18n("Manual Configuration")

    readonly property Connections manuConfigurationConnections: Connections {
        target: SetupManager.manualConfiguration

        function onIncomingHostNameChanged(): void {
            manualIncomingHostName.text = SetupManager.manualConfiguration.incomingHostName;
        }

        function onOutgoingHostNameChanged(): void {
            manualOutgoingHostName.text = SetupManager.manualConfiguration.outgoingHostName;
        }

        function onIncomingUserNameChanged(): void {
            manualIncomingUserName.text = SetupManager.manualConfiguration.incomingUserName;
        }
        function onOutgoingUserNameChanged(): void {
            manualOutgoingUserName.text = SetupManager.manualConfiguration.outgoingUserName;
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
            text: SetupManager.manualConfiguration.incomingHostName;
            onTextEdited: {
                SetupManager.manualConfiguration.incomingHostName = manualIncomingHostName.text
            }
        }

        FormCard.FormComboBoxDelegate {
            id: manualIncomingProtocol
            text: i18n("Protocol:")
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
            text: i18n("Security:")
            model: SetupManager.manualConfiguration.securityProtocols
            currentIndex: SetupManager.manualConfiguration.currentIncomingSecurityProtocol
            onCurrentIndexChanged: {
                SetupManager.manualConfiguration.currentIncomingSecurityProtocol = currentIndex
            }
        }
        FormCard.FormComboBoxDelegate {
            id: manualIncomingAuthenticationMethod
            text: i18n("Authentication Method:")
            textRole: "text"
            valueRole: "value"
            model: [
                { value: LoginJob.ClearText, text: i18n("Clear text") },
                { value: LoginJob.Login, text: i18n("LOGIN") },
                { value: LoginJob.CramMD5, text: i18n("CRAM-MD5") },
                { value: LoginJob.DigestMD5, text: i18n("DIGEST-MD5") },
                { value: LoginJob.NTLM, text: i18n("NTLM") },
                { value: LoginJob.GSSAPI, text: i18n("GSSAPI") },
                { value: LoginJob.XOAuth2, text: i18n("XOAuth (Gmail)") },
            ]
            currentIndex: indexOfValue(SetupManager.manualConfiguration.currentIncomingAuthenticationProtocol)
            onCurrentIndexChanged: {
                SetupManager.manualConfiguration.currentIncomingAuthenticationProtocols = currentValue
            }
        }
        FormCard.FormTextFieldDelegate {
            id: manualIncomingUserName
            label: i18n("Username:")
            inputMethodHints: Qt.ImhUrlCharactersOnly
            text: SetupManager.manualConfiguration.incomingUserName
            onTextEdited: {
                SetupManager.manualConfiguration.incomingUserName = manualIncomingUserName.text
            }
        }

        FormCard.FormCheckDelegate {
            id: disconnectedModeEnabled
            visible: SetupManager.manualConfiguration.hasDisconnectedMode
            description: i18n("Download all messages for offline use")
            onCheckedChanged: {
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
            onTextEdited: {
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
            text: i18n("Security:")
            model: SetupManager.manualConfiguration.securityProtocols
            currentIndex: SetupManager.manualConfiguration.currentOutgoingSecurityProtocol
            onCurrentIndexChanged: {
                SetupManager.manualConfiguration.currentOutgoingSecurityProtocol = currentIndex
            }
        }
        FormCard.FormComboBoxDelegate {
            id: manualOutgoingAuthenticationMethod
            text: i18n("Authentication Method:")
            textRole: "text"
            valueRole: "value"
            model: [
                { value: LoginJob.ClearText, text: i18n("Clear text") },
                { value: LoginJob.Plain, text: i18n("PLAIN") },
                { value: LoginJob.Login, text: i18n("LOGIN") },
                { value: LoginJob.CramMD5, text: i18n("CRAM-MD5") },
                { value: LoginJob.DigestMD5, text: i18n("DIGEST-MD5") },
                { value: LoginJob.NTLM, text: i18n("NTLM") },
                { value: LoginJob.GSSAPI, text: i18n("GSSAPI") },
                { value: LoginJob.XOAuth2, text: i18n("XOAuth (Gmail)") },
            ]
            currentIndex: indexOfValue(SetupManager.manualConfiguration.currentOutgoingAuthenticationProtocol)
            onCurrentIndexChanged: {
                SetupManager.manualConfiguration.currentOutgoingAuthenticationProtocols = currentValue
            }
        }
        FormCard.FormTextFieldDelegate {
            id: manualOutgoingUserName
            label: i18n("Username:")
            inputMethodHints: Qt.ImhUrlCharactersOnly
            text: SetupManager.manualConfiguration.outgoingUserName
            onTextEdited: {
                SetupManager.manualConfiguration.outgoingUserName = manualOutgoingUserName.text
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
                SetupManager.manualConfiguration.checkServer()
            }
            visible: manualConfiguration.visible
            enabled: SetupManager.manualConfiguration.configurationIsValid && !SetupManager.manualConfiguration.serverTestInProgress
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
