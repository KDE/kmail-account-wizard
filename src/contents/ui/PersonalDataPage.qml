// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15 as QQC2
import org.kde.kirigami 2.15 as Kirigami
import org.kde.pim.accountwizard 1.0

Kirigami.ScrollablePage {
    id: root
    title: i18n("Personal Information")
    
    leftPadding: 20

    ColumnLayout {
        spacing: Kirigami.Units.largeSpacing
        Kirigami.Heading {
            Layout.alignment: Qt.AlignHCenter
            Layout.maximumWidth: Kirigami.Units.gridUnit * 25
            Layout.fillWidth: true
            horizontalAlignment: form.wideMode ? Qt.AlignHCenter : Qt.AlignLeft
            level: 2
            text: i18n("With a few simple steps we create the right settings for you. Please follow the steps of this wizard carefully.")
            wrapMode: Text.WordWrap
        }
        Kirigami.FormLayout {
            id: form
            Layout.fillWidth: true

            QQC2.TextField {
                Kirigami.FormData.label: i18n("Full name:")
                placeholderText: i18nc("Generic name", "John Smith") 
                text: SetupManager.name()
            }
            QQC2.TextField {
                Kirigami.FormData.label: i18n("E-mail address:")
                placeholderText: i18nc("Generic email address", "boss@example.corp")
                text: SetupManager.email()
            }
            Kirigami.PasswordField {
                Kirigami.FormData.label: i18n("Password:")
            }
            
            QQC2.CheckBox {
                text: i18n("Find provider settings on the internet")
            }
        }
        QQC2.Label {
            Layout.alignment: Qt.AlignHCenter
            Layout.maximumWidth: Kirigami.Units.gridUnit * 25
            Layout.fillWidth: true
            text: i18n("Check online for the settings needed for this email provider. Only the domain name part of the e-mail address will be sent over the Internet at this point. If this option is unchecked, the account can be set up manually.")
            wrapMode: Text.WordWrap
        }
        Component {
            id: accountSelectionPageComponent
            AccountSelectionPage {}
        }
    }
    
    
    footer: QQC2.ToolBar {
        contentItem: RowLayout {
            QQC2.Button {
                Layout.alignment: Qt.AlignRight
                text: i18n("Next")
                onClicked: QQC2.ApplicationWindow.window.pageStack.push(accountSelectionPageComponent);
            }
        }
    }
}
