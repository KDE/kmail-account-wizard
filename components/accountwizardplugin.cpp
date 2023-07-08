// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "accountwizardplugin.h"

#include <QQmlEngine>

void AccountWizardPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QByteArray("org.kde.pim.accountwizard.components"));
    qmlRegisterModule(uri, 1, 0);
}

#include "moc_accountwizardplugin.cpp"
