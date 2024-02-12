// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick
import QtQuick.Controls as QQC2
import org.kde.kirigami as Kirigami
import org.kde.pim.accountwizard as Account

Kirigami.ApplicationWindow {
    id: root

    width: Kirigami.Units.gridUnit * 45
    height: Kirigami.Units.gridUnit * 36

    pageStack {
        initialPage: PersonalDataPage {}
        defaultColumnWidth: root.width

        globalToolBar {
            style: Kirigami.ApplicationHeaderStyle.ToolBar
            showNavigationButtons: if (applicationWindow().pageStack.currentIndex > 0) {
                Kirigami.ApplicationHeaderStyle.ShowBackButton
            } else {
                0
            }
        }
    }
}
