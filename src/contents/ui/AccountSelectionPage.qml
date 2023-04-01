// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15 as QQC2
import org.kde.kirigami 2.15 as Kirigami
import org.kde.pim.accountwizard 1.0 as Account

Kirigami.ScrollablePage {
    title: i18n("Select Account Type")
    ListView {
        model: Account.WizardModel {
            id: wizardModel
        }
        delegate: Kirigami.BasicListItem {
            required property int index
            required property string description
            required property string name
            required property string iconName
            required property string pluginId

            icon: iconName
            label: name
            subtitle: description

            onClicked: {
                Account.Controller.wizardId = pluginId;
                pageStack.push(Account.Controller.wizardItem, {
                    title: name,
                });
            }
        }
    }
}
