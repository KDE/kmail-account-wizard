// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
// SPDX-FileCopyrightText: 2023-2026 Laurent Montel <montel@kde.org>
// SPDX-FileCopyrightText: 2026 Alan Thouvenin <ath@enioka.com>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick
import org.kde.ki18n
import QtQuick.Layouts
import QtQuick.Controls as QQC2
import org.kde.kirigami as Kirigami
import org.kde.pim.accountwizard
import org.kde.kirigamiaddons.formcard as FormCard
import org.kde.kirigamiaddons.components as Component

WizardPage {
    id: root

    property bool loading: false;

    title: KI18n.i18nc("@title:group", "Configuration Selection")

    LoadingOverlay {
        parent: applicationWindow().contentItem
        loading: root.loading
    }

    nextAction {
        enabled: configurationGroup.checkedButton !== null
        onTriggered: {
            if (configurationGroup.checkedButton === configureManual) {
                applicationWindow().pageStack.push(Qt.createComponent('org.kde.pim.accountwizard', 'ManualConfigurationPage'));
            } else {
                root.loading = true;
                SetupManager.configurationModel.testLogin(configurationGroup.checkedButton.index);
            }
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
                SetupManager.configurationModel.createAutomaticAccount(configurationGroup.checkedButton.index, ConsoleLog, calendarCheck.checked);
            } else {
                applicationWindow().pageStack.push(
                    Qt.createComponent('org.kde.pim.accountwizard', 'FixPasswordPage'), {
                        index: configurationGroup.checkedButton.index,
                        calendarChecked: calendarCheck.checked
                });
            }
            root.loading = false;
        }
    }

    FormCard.FormHeader {
        title: KI18n.i18n("Available configurations")
    }

    FormCard.FormCard {
        id: availableConfigurations

        autoSeparators: true

        QQC2.ButtonGroup {
            id: configurationGroup
        }

        FormCard.AbstractFormDelegate {
            Layout.preferredHeight: Kirigami.Units.gridUnit * 10

            contentItem: Item {
                Kirigami.PlaceholderMessage {
                    anchors.centerIn: parent
                    width: parent.width - Kirigami.Units.gridUnit * 2
                    text: KI18n.i18n("No configuration found from the internet for this server.")
                }
            }

            background: null

            visible: SetupManager.noConfigFound
        }

        Repeater {
            id: configurationRepeater
            model: SetupManager.configurationModel

            delegate: ConfigurationDelegate {
                id: configurationDelegate

                required property int index

                checked: index === 0

                QQC2.ButtonGroup.group: configurationGroup
            }
        }

        FormCard.FormRadioDelegate {
            id: configureManual
            text: KI18n.i18n("Manual Configuration")

            checked: SetupManager.noConfigFound

            QQC2.ButtonGroup.group: configurationGroup
        }
    }

    FormCard.FormHeader {
        visible: calendarContactCard.visible
        title: KI18n.i18nc("@title:group", "Calendar and Contacts")
    }

    FormCard.FormCard {
        id: calendarContactCard

        visible: SetupManager.configurationModel.hasGroupwareSupport

        FormCard.FormSwitchDelegate {
            id: calendarCheck

            text: KI18n.i18nc("@option:check", "Enable calendar and contact integration")
            checked: visible
        }
    }
}
