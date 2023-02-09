// SDPX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
// SPDX-LicenseRed: LGPL-2.0-or-later

#include "wizardpackage.h"

#include <kpackage/package.h>

void WizardPackageStructure::initPackage(KPackage::Package *package)
{
    package->setDefaultPackageRoot(QStringLiteral("wizard/packages"));
    package->addDirectoryDefinition("ui", QStringLiteral("ui"), i18n("User Interface"));

    package->addFileDefinition("mainscript", QStringLiteral("ui/main.qml"), i18n("Main Script File"));
    package->setRequired("mainscript", true);
}

K_PLUGIN_CLASS_WITH_JSON(WizardPackageStructure, "wizard-packagestructure.json")

#include "wizardpackage.moc"
