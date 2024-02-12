// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
// SPDX-FileCopyrightText: 2023-2024 Laurent Montel <montel.org>
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

    title: i18nc("@title:group", "Configuration Selection")

    nextAction {
        enabled: configurationGroup.checkedButton !== null
        onTriggered: {
            if (configurationGroup.checkedButton === configureManual) {
                applicationWindow().pageStack.push(Qt.createComponent('org.kde.pim.accountwizard', 'ManualConfigurationPage'));
            } else {
                applicationWindow().pageStack.push(Qt.createComponent('org.kde.pim.accountwizard', 'DetailsPage'));
                SetupManager.configurationModel.createAutomaticAccount(configurationGroup.checkedButton.index);
            }
        }
    }

    header: Component.Banner {
        id: ispdbSearchInfo
        width: parent.width
        type: Kirigami.MessageType.Information
        text: SetupManager.searchIspdbFoundMessage
        visible: SetupManager.searchIspdbFoundMessage.length > 0
    }

    FormCard.FormHeader {
        title: i18n("Available configurations")
    }

    FormCard.FormCard {
        id: availableConfigurations

        QQC2.ButtonGroup {
            id: configurationGroup
        }

        FormCard.AbstractFormDelegate {
            Layout.preferredHeight: Kirigami.Units.gridUnit * 10

            contentItem: Item {
                Kirigami.PlaceholderMessage {
                    anchors.centerIn: parent
                    width: parent.width - Kirigami.Units.gridUnit * 2
                    text: i18n("No configuration found from the internet for this server.")
                }
            }

            background: null

            visible: SetupManager.noConfigFound
        }

        Repeater {
            id: configurationRepeater
            model: SetupManager.configurationModel

            delegate: ColumnLayout {
                id: configurationDelegate

                spacing: 0

                required property int index
                required property string name
                required property string description
                required property string incomingHost
                required property var incomingTags
                required property string outgoingHost
                required property var outgoingTags

                FormCard.FormDelegateSeparator { visible: configurationDelegate.index !== 0}

                ConfigurationDelegate {
                    name: configurationDelegate.name
                    description: configurationDelegate.description
                    incomingHost: configurationDelegate.incomingHost
                    incomingTags: configurationDelegate.incomingTags
                    outgoingHost: configurationDelegate.outgoingHost
                    outgoingTags: configurationDelegate.outgoingTags

                    checked: index === 0

                    QQC2.ButtonGroup.group: configurationGroup
                }
            }
        }

        FormCard.FormDelegateSeparator {
            visible: configurationRepeater.count > 0 || SetupManager.noConfigFound
        }

        FormCard.FormRadioDelegate {
            id: configureManual
            text: i18n("Manual Configuration")

            checked: SetupManager.noConfigFound

            QQC2.ButtonGroup.group: configurationGroup
        }
    }
}
