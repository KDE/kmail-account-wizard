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

    actions: Kirigami.Action {
        text: i18n("Recheck")
        checked: true
        onTriggered: manualConfiguration.checkServer()
        enabled: manualConfiguration.configurationIsValid && !manualConfiguration.serverTestInProgress
    }

    nextAction {
        text: i18n("Create Account")
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
        title: i18n("Incoming Server Parameters")
    }

    FormCard.FormCard {
        FormCard.FormTextFieldDelegate {
            id: manualIncomingHostName
            label: i18n("Incoming server:")
            inputMethodHints: Qt.ImhUrlCharactersOnly
            text: manualConfiguration.incomingHostName;
            onTextChanged: {
                manualConfiguration.incomingHostName = manualIncomingHostName.text
            }
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormComboBoxDelegate {
            id: manualIncomingProtocol
            text: i18n("Protocol:")
            textRole: "text"
            valueRole: "value"
            model: [
                { value: AccountConfiguration.IMAP, text: i18n("IMAP") },
                { value: AccountConfiguration.POP3, text: i18n("POP3") },
                { value: AccountConfiguration.KOLAB, text: i18n("Kolab") },
            ]
            Component.onCompleted: {
                currentIndex = indexOfValue(manualConfiguration.incomingProtocol);
            }
            onCurrentIndexChanged: {
                manualConfiguration.incomingProtocol = model[currentIndex].value;
            }
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormSpinBoxDelegate {
            id: manualIncomingPort
            label: i18n("Port:")
            value: manualConfiguration.incomingPort
            from: 1
            to: 9999
            onValueChanged: manualConfiguration.incomingPort = value
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormComboBoxDelegate {
            id: manualIncomingSecurity
            text: i18n("Security:")
            textRole: "text"
            valueRole: "value"
            model: [
                { value: Transport.SSL, text: i18n("SSL/TLS (recommended)") },
                { value: Transport.TLS, text: i18n("StartTLS") },
                { value: Transport.None, text: i18n("None") }
            ]
            Component.onCompleted: {
                currentIndex = indexOfValue(manualConfiguration.incomingSecurityProtocol);
            }
            onCurrentIndexChanged: {
                manualConfiguration.incomingSecurityProtocol = model[currentIndex].value;
            }
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormComboBoxDelegate {
            id: manualIncomingAuthenticationMethod
            text: i18n("Authentication Method:")
            textRole: "text"
            valueRole: "value"
            model: [
                { value: Transport.CLEAR, text: i18n("Clear text") },
                { value: Transport.PLAIN, text: i18n("PLAIN") },
                { value: Transport.LOGIN, text: i18n("LOGIN") },
                { value: Transport.CRAM_MD5, text: i18n("CRAM-MD5") },
                { value: Transport.CRAM_MD5, text: i18n("DIGEST-MD5") },
                { value: Transport.NTLM, text: i18n("NTLM") },
                { value: Transport.GSSAPI, text: i18n("GSSAPI") },
                { value: Transport.XOAuth2, text: i18n("XOAuth (Gmail)") },
                { value: Transport.APOP, text: i18n("APOP") },
            ]
            Component.onCompleted: {
                currentIndex = indexOfValue(manualConfiguration.incomingAuthenticationProtocol);
            }
            onCurrentIndexChanged: {
                manualConfiguration.incomingAuthenticationProtocol = model[currentIndex].value;
            }
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormTextFieldDelegate {
            id: manualIncomingUserName
            label: i18n("Username:")
            inputMethodHints: Qt.ImhUrlCharactersOnly
            text: manualConfiguration.incomingUserName
            onTextChanged: {
                manualConfiguration.incomingUserName = manualIncomingUserName.text
            }
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormCheckDelegate {
            id: disconnectedModeEnabled
            visible: manualConfiguration.hasDisconnectedMode
            description: i18n("Download all messages for offline use")
            onCheckedChanged: {
                manualConfiguration.disconnectedModeEnabled = checked
            }
            checked: manualConfiguration.disconnectedModeEnabled
        }
    }

    FormCard.FormHeader {
        title: i18n("Outgoing Server Parameters")
    }

    FormCard.FormCard {

        FormCard.FormTextFieldDelegate {
            id: manualOutgoingHostName
            label: i18n("Outgoing server:")
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
            label: i18n("Port:")
            value: manualConfiguration.mailTransport.port
            from: 1
            to: 9999
            onValueChanged: {
                manualConfiguration.mailTransport.port = value;
                manualConfiguration.checkConfiguration();
            }
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormComboBoxDelegate {
            id: manualOutgoingSecurity
            text: i18n("Security:")
            textRole: "text"
            valueRole: "value"
            model: [
                { value: Transport.None, text: i18n("None") },
                { value: Transport.SSL, text: i18n("SSL/TLS (recommended)") },
                { value: Transport.TLS, text: i18n("StartTLS") }
            ]
            onCurrentIndexChanged: {
                manualConfiguration.mailTransport.encryption = currentIndex
            }
            Component.onCompleted: currentIndex = indexOfValue(manualConfiguration.mailTransport.encryption)
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormComboBoxDelegate {
            id: manualOutgoingAuthenticationMethod
            text: i18n("Authentication Method:")
            textRole: "text"
            valueRole: "value"
            model: [
                { value: Transport.CLEAR, text: i18n("Clear text") },
                { value: Transport.PLAIN, text: i18n("PLAIN") },
                { value: Transport.LOGIN, text: i18n("LOGIN") },
                { value: Transport.CRAM_MD5, text: i18n("CRAM-MD5") },
                { value: Transport.CRAM_MD5, text: i18n("DIGEST-MD5") },
                { value: Transport.NTLM, text: i18n("NTLM") },
                { value: Transport.GSSAPI, text: i18n("GSSAPI") },
                { value: Transport.XOAuth2, text: i18n("XOAuth (Gmail)") },
                { value: Transport.APOP, text: i18n("APOP") },
            ]
            Component.onCompleted: currentIndex = indexOfValue(manualConfiguration.mailTransport.authenticationType);

            onCurrentIndexChanged: {
                manualConfiguration.mailTransport.authenticationType = model[currentIndex].value;
            }
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormTextFieldDelegate {
            id: manualOutgoingUserName
            label: i18n("Username:")
            inputMethodHints: Qt.ImhUrlCharactersOnly
            text: manualConfiguration.mailTransport.userName
            onTextChanged: {
                manualConfiguration.mailTransport.userName = manualOutgoingUserName.text;
                manualConfiguration.checkConfiguration();
            }
        }
    }
}
