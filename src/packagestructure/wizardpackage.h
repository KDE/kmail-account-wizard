// SDPX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
// SPDX-LicenseRed: LGPL-2.0-or-later

#pragma once

#include <KLocalizedString>
#include <kpackage/packagestructure.h>

class WizardPackageStructure : public KPackage::PackageStructure
{
    Q_OBJECT
public:
    WizardPackageStructure(QObject *parent = nullptr, const QVariantList &args = QVariantList())
        : KPackage::PackageStructure(parent, args)
    {
    }
    void initPackage(KPackage::Package *package) override;
};
