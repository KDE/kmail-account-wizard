// SPDX-FileCopyrightText: 2026 Alan Thouvenin <ath@enioka.com>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick
import QtQuick.Controls as QQC2

Item {
    id: root

    property bool loading: false

    visible: loading
    anchors.fill: parent

    Rectangle {
        anchors.fill: parent
        color: "black"
        opacity: 0.35
    }

    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.AllButtons
        hoverEnabled: true
        preventStealing: true
    }

    QQC2.BusyIndicator {
        anchors.centerIn: parent
        running: root.loading
    }
}
