// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
// SPDX-FileCopyrightText: 2023-2026 Laurent Montel <montel@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick
import org.kde.ki18n
import QtQuick.Layouts
import QtQuick.Controls as QQC2
import org.kde.kirigami as Kirigami
import org.kde.pim.accountwizard
import org.kde.kirigamiaddons.formcard as FormCard

WizardPage {
    id: root

    title: KI18n.i18n("Manual Configuration")

    actions: Kirigami.Action {
        text: KI18n.i18n("Recheck")
        checked: true
        onTriggered: manualConfiguration.checkServer()
        enabled: manualConfiguration.configurationIsValid && !manualConfiguration.serverTestInProgress
    }

    nextAction {
        text: KI18n.i18n("Create Account")
        onTriggered: {
            applicationWindow().pageStack.push(Qt.createComponent('org.kde.pim.accountwizard', 'DetailsPage'));
            manualConfiguration.save(ConsoleLog);
        }
        enabled: manualConfiguration.configurationIsValid
    }

    AccountConfiguration {
        id: manualConfiguration

        password: SetupManager.password
        email: SetupManager.email
        identity.fullName: SetupManager.fullName
    }

    Connections {
        target: manualConfiguration

        function onIncomingHostNameChanged(): void {
            manualIncomingHostName.text = manualConfiguration.incomingHostName;
        }

        function onIncomingUserNameChanged(): void {
            manualIncomingUserName.text = manualConfiguration.incomingUserName;
        }
    }

    Connections {
        target: manualConfiguration.mailTransport

        function onUserNameChanged(): void {
            manualOutgoingUserName.text = manualConfiguration.mailTransport.userName;
        }

        function onHostChanged(): void {
            manualOutgoingHostName.text = manualConfiguration.mailTransport.host;
        }
    }


    FormCard.FormHeader {
        title: KI18n.i18n("Incoming Server Parameters")
    }

    FormCard.FormCard {
        FormCard.FormTextFieldDelegate {
            id: manualIncomingHostName
            label: KI18n.i18n("Incoming server:")
            inputMethodHints: Qt.ImhUrlCharactersOnly
            text: manualConfiguration.incomingHostName;
            onTextChanged: {
                manualConfiguration.incomingHostName = manualIncomingHostName.text
            }
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormComboBoxDelegate {
            id: manualIncomingProtocol
            text: KI18n.i18n("Protocol:")
            textRole: "text"
            valueRole: "value"
            model: [
                { value: AccountConfiguration.IMAP, text: KI18n.i18n("IMAP") },
                { value: AccountConfiguration.POP3, text: KI18n.i18n("POP3") },
            ]
            Binding {
                target: manualIncomingProtocol
                property: "currentIndex"
                value: manualIncomingProtocol.model.findIndex(entry => entry.value === manualConfiguration.incomingProtocol)
                delayed: true
            }
            onActivated: index => {
                manualConfiguration.incomingProtocol = manualIncomingProtocol.model[index].value;
            }
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormSpinBoxDelegate {
            id: manualIncomingPort
            label: KI18n.i18n("Port:")
            value: manualConfiguration.incomingPort
            from: 1
            to: 65535
            onValueChanged: manualConfiguration.incomingPort = value
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormComboBoxDelegate {
            id: manualIncomingSecurity
            text: KI18n.i18n("Security:")
            textRole: "text"
            valueRole: "value"
            model: [
                { value: Transport.SSL, text: KI18n.i18n("SSL/TLS (recommended)") },
                { value: Transport.TLS, text: KI18n.i18n("StartTLS") },
                { value: Transport.None, text: KI18n.i18n("None") }
            ]
            Binding {
                target: manualIncomingSecurity
                property: "currentIndex"
                value: manualIncomingSecurity.model.findIndex(entry => entry.value === manualConfiguration.incomingSecurityProtocol)
                delayed: true
            }
            onActivated: index => {
                manualConfiguration.incomingSecurityProtocol = manualIncomingSecurity.model[index].value;
            }
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormComboBoxDelegate {
            id: manualIncomingAuthenticationMethod
            text: KI18n.i18n("Authentication Method:")
            textRole: "text"
            valueRole: "value"

            model: {
                let model = [
                       { value: Transport.CLEAR, text: KI18n.i18n("Normal Password") },
                       { value: Transport.PLAIN, text: KI18n.i18n("PLAIN") },
                       { value: Transport.LOGIN, text: KI18n.i18n("LOGIN") },
                       { value: Transport.CRAM_MD5, text: KI18n.i18n("CRAM-MD5") },
                       { value: Transport.DIGEST_MD5, text: KI18n.i18n("DIGEST-MD5") },
                       { value: Transport.NTLM, text: KI18n.i18n("NTLM") },
                       { value: Transport.GSSAPI, text: KI18n.i18n("Kerberos / GSSAPI") },
                       { value: Transport.XOAUTH2, text: KI18n.i18n("XOAuth (Gmail)") },
                ];
                if (manualConfiguration.incomingProtocol == AccountConfiguration.POP3) {
                    model.push({ value: Transport.APOP, text: KI18n.i18n("APOP") });
                }
                return model;
            }
            Binding {
                target: manualIncomingAuthenticationMethod
                property: "currentIndex"
                value: manualIncomingAuthenticationMethod.model.findIndex(entry => entry.value === manualConfiguration.incomingAuthenticationProtocol)
                delayed: true
            }
            onActivated: index => {
                manualConfiguration.incomingAuthenticationProtocol = manualIncomingAuthenticationMethod.model[index].value;
            }
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormTextFieldDelegate {
            id: manualIncomingUserName
            label: KI18n.i18n("Username:")
            inputMethodHints: Qt.ImhUrlCharactersOnly
            text: manualConfiguration.incomingUserName
            onTextChanged: {
                manualConfiguration.incomingUserName = manualIncomingUserName.text
            }
        }

        FormCard.FormDelegateSeparator {
            visible: manualConfiguration.hasDisconnectedMode
        }

        FormCard.FormCheckDelegate {
            id: disconnectedModeEnabled
            visible: manualConfiguration.hasDisconnectedMode
            text: KI18n.i18n("Download all messages for offline use")
            onCheckedChanged: {
                manualConfiguration.disconnectedModeEnabled = checked
            }
            checked: manualConfiguration.disconnectedModeEnabled
        }
    }

    FormCard.FormHeader {
        title: KI18n.i18n("Outgoing Server Parameters")
    }

    FormCard.FormCard {

        FormCard.FormTextFieldDelegate {
            id: manualOutgoingHostName
            label: KI18n.i18n("Outgoing server:")
            inputMethodHints: Qt.ImhUrlCharactersOnly
            text: manualConfiguration.mailTransport.host
            onTextChanged: {
                manualConfiguration.mailTransport.host = text;
                manualConfiguration.checkConfiguration();
            }
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormSpinBoxDelegate {
            id: manualOutgoingPort
            label: KI18n.i18n("Port:")
            value: manualConfiguration.mailTransport.port
            from: 1
            to: 65535
            onValueChanged: {
                manualConfiguration.mailTransport.port = value;
                manualConfiguration.checkConfiguration();
            }
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormComboBoxDelegate {
            id: manualOutgoingSecurity
            text: KI18n.i18n("Security:")
            textRole: "text"
            valueRole: "value"
            model: [
                { value: Transport.None, text: KI18n.i18n("None") },
                { value: Transport.SSL, text: KI18n.i18n("SSL/TLS (recommended)") },
                { value: Transport.TLS, text: KI18n.i18n("StartTLS") }
            ]
            Binding {
                target: manualOutgoingSecurity
                property: "currentIndex"
                value: manualOutgoingSecurity.model.findIndex(entry => entry.value === manualConfiguration.mailTransport.encryption)
                delayed: true
            }
            onActivated: index => {
                manualConfiguration.mailTransport.encryption = manualOutgoingSecurity.model[index].value;
            }
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormComboBoxDelegate {
            id: manualOutgoingAuthenticationMethod
            text: KI18n.i18n("Authentication Method:")
            textRole: "text"
            valueRole: "value"
            model: [
                { value: Transport.CLEAR, text: KI18n.i18n("Clear text") },
                { value: Transport.PLAIN, text: KI18n.i18n("PLAIN") },
                { value: Transport.LOGIN, text: KI18n.i18n("LOGIN") },
                { value: Transport.CRAM_MD5, text: KI18n.i18n("CRAM-MD5") },
                { value: Transport.DIGEST_MD5, text: KI18n.i18n("DIGEST-MD5") },
                { value: Transport.NTLM, text: KI18n.i18n("NTLM") },
                { value: Transport.GSSAPI, text: KI18n.i18n("GSSAPI") },
                { value: Transport.XOAUTH2, text: KI18n.i18n("XOAuth (Gmail)") },
            ]
            Binding {
                target: manualOutgoingAuthenticationMethod
                property: "currentIndex"
                value: manualOutgoingAuthenticationMethod.model.findIndex(entry => entry.value === manualConfiguration.mailTransport.authenticationType)
                delayed: true
            }
            onActivated: index => {
                manualConfiguration.mailTransport.authenticationType = manualOutgoingAuthenticationMethod.model[index].value;
            }
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormTextFieldDelegate {
            id: manualOutgoingUserName
            label: KI18n.i18n("Username:")
            inputMethodHints: Qt.ImhUrlCharactersOnly
            text: manualConfiguration.mailTransport.userName
            onTextChanged: {
                manualConfiguration.mailTransport.userName = manualOutgoingUserName.text;
                manualConfiguration.checkConfiguration();
            }
        }
    }
}
