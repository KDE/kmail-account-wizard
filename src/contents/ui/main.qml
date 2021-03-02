// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>

import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC2
import org.kde.kirigami 2.15 as Kirigami
import org.kde.pim.accountwizard 1.0 as Account

Kirigami.ApplicationWindow {
    Account.WizardModel {
        id: wizardModel
    }
    pageStack.initialPage: Kirigami.ScrollablePage {
        title: i18n("Select Account Type")
        ListView {
            model: wizardModel
            delegate: Kirigami.BasicListItem {
                icon: model.decoration
                label: model.display
                subtitle: model.tooltip

                onClicked: {
                    Account.Controller.wizardId = model.pluginId;
                    pageStack.push(Account.Controller.wizardItem, {"title": model.display});
                }
            }
        }
    }
}
