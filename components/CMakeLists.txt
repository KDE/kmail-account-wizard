# SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
# SPDX-License-Identifier: BSD-2-Clause

### QML plugin

ecm_add_qml_module(accountwizardplugin URI "org.kde.pim.accountwizard.components" VERSION 1.0)

target_sources(accountwizardplugin PRIVATE
    accountwizardplugin.h
    accountwizardplugin.cpp
)

target_link_libraries(accountwizardplugin PRIVATE Qt::Qml)

ecm_target_qml_sources(accountwizardplugin SOURCES
    BasePage.qml
    InfoDetail.qml
)

ecm_finalize_qml_module(accountwizardplugin
    DESTINATION ${KDE_INSTALL_QMLDIR}
    BUILD_SHARED_LIBS OFF)
