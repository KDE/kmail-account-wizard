// SPDX-FileCopyrightText: 2022 Devin Lin <devin@kde.org>
// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick 2.15
import QtQuick.Templates 2.15 as T
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15

import org.kde.kirigami 2.19 as Kirigami
import org.kde.kirigamiaddons.formcard 1.0 as FormCard

/**
 * Form delegate that corresponds to a checkbox.
 */
T.RadioDelegate {
    id: root

    required property string name
    required property string description
    required property string incomingHost
    required property var incomingTags
    required property string outgoingHost
    required property var outgoingTags

    leftPadding: Kirigami.Units.gridUnit
    topPadding: Kirigami.Units.largeSpacing + Kirigami.Units.smallSpacing
    bottomPadding: Kirigami.Units.largeSpacing + Kirigami.Units.smallSpacing
    rightPadding: Kirigami.Units.gridUnit

    implicitWidth: contentItem.implicitWidth + leftPadding + rightPadding
    implicitHeight: contentItem.implicitHeight + topPadding + bottomPadding

    focusPolicy: Qt.StrongFocus
    hoverEnabled: true

    background: FormCard.FormDelegateBackground { control: root }

    Layout.fillWidth: true

    contentItem: RowLayout {
        spacing: 0

        QQC2.RadioButton {
            id: checkBoxItem
            Layout.rightMargin: Kirigami.Units.largeSpacing + Kirigami.Units.smallSpacing
            Layout.alignment: Qt.AlignTop
            focusPolicy: Qt.NoFocus // provided by delegate

            onToggled: {
                root.toggle();
                root.toggled();
            }
            onClicked: root.clicked()
            onPressAndHold: root.pressAndHold()
            onDoubleClicked: root.doubleClicked()

            enabled: root.enabled
            checked: root.checked
        }

        ColumnLayout {
            Layout.fillWidth: true
            spacing: Kirigami.Units.smallSpacing

            Behavior on height {
                NumberAnimation { duration: 400 }
            }

            Kirigami.Heading {
                level: 4
                text: root.name
                color: root.enabled ? Kirigami.Theme.textColor : Kirigami.Theme.disabledTextColor
                elide: Text.ElideRight
                font.bold: true
                wrapMode: Text.Wrap
                Layout.fillWidth: true
            }

            QQC2.Label {
                Layout.fillWidth: true
                text: root.description
                wrapMode: Text.Wrap
            }

            GridLayout {
                id: details
                columns: 2
                visible: checkBoxItem.checked

                QQC2.Label {
                    font.bold: true
                    text: i18n("Incoming")
                }

                RowLayout {
                    Repeater {
                        model: root.incomingTags
                        Kirigami.Chip {
                            text: modelData
                            enabled: false
                            closable: false
                        }
                    }
                }

                QQC2.Label {
                    text: root.incomingHost
                    Layout.columnSpan: 2
                }

                QQC2.Label {
                    font.bold: true
                    text: i18n("Outgoing")
                    visible: root.outgoingTags.length > 0
                }

                RowLayout {
                    Repeater {
                        model: root.outgoingTags
                        Kirigami.Chip {
                            text: modelData
                            enabled: false
                            closable: false
                        }
                    }
                }

                QQC2.Label {
                    text: root.outgoingHost
                    Layout.columnSpan: 2
                }
            }
        }
    }
}

