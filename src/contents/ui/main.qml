// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC2
import org.kde.kirigami 2.15 as Kirigami
import org.kde.pim.accountwizard 1.0 as Account

Kirigami.ApplicationWindow {
    id: root

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
