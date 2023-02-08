# SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
# SPDX-License-Identifier: BSD-2-Clause

add_library(wizard_packagestructure MODULE wizardpackage.cpp wizardpackage.h)
target_link_libraries(wizard_packagestructure PRIVATE KF${KF_MAJOR_VERSION}::Package KF${KF_MAJOR_VERSION}::I18n)
install(TARGETS wizard_packagestructure DESTINATION ${KDE_INSTALL_PLUGINDIR}/kpackage/packagestructure)
