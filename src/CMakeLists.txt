# SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
# SPDX-FileCopyrightText: 2023-2024 Laurent Montel <montel.org>
# SPDX-License-Identifier: BSD-3-Clause

## Static lib target
qt_add_library(accountwizard_static STATIC)

qt_add_qml_module(accountwizard_static
    URI org.kde.pim.accountwizard
    QML_FILES
        qml/ConfigurationDelegate.qml
        qml/ConfigurationSelectionPage.qml
        qml/DetailsPage.qml
        qml/Main.qml
        qml/ManualConfigurationPage.qml
        qml/PersonalDataPage.qml
        qml/WizardPage.qml
    SOURCES
        consolelog.cpp
        consolelog.h
        foreigntypes.h

        ispdb/configurationmodel.h
        ispdb/configurationmodel.cpp
        ispdb/serverconfiguration.h
        ispdb/serverconfiguration.cpp
        ispdbservice.h
        ispdbservice.cpp
        manualconfiguration.h
        manualconfiguration.cpp
        resource.h
        resource.cpp
        setupmanager.h
        setupmanager.cpp
        servertest.h
        servertest.cpp
    VERSION
        1.0
)

include_directories(ispdb)

ecm_qt_declare_logging_category(accountwizard_static
    HEADER accountwizard_debug.h
    IDENTIFIER ACCOUNTWIZARD_LOG
    CATEGORY_NAME org.kde.pim.accountwizard
    OLD_CATEGORY_NAMES log_accountwizard
    DESCRIPTION "accountwizard (accountwizard)"
    EXPORT ACCOUNTWIZARD
)


target_link_libraries(accountwizard_static PUBLIC
    KPim6::AkonadiCore
    KPim6::AkonadiWidgets
    KPim6::MailTransport
    KPim6::Mime
    KPim6::IMAP
    KPim6::IdentityManagementCore
    KF6::KIOCore
    KF6::WidgetsAddons
    KF6::I18n
    KF6::Crash
    KF6::DBusAddons
    Qt::Xml
    Qt::Quick
    Qt::QuickControls2
)

if (COMPILE_WITH_UNITY_CMAKE_SUPPORT)
    set_target_properties(accountwizard_static PROPERTIES UNITY_BUILD ON)
endif()

## Binary target
add_executable(accountwizard
    main.cpp
)

target_link_libraries(accountwizard PRIVATE accountwizard_static accountwizard_staticplugin)

## Install
install(TARGETS accountwizard ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
install(PROGRAMS org.kde.accountwizard.desktop DESTINATION ${KDE_INSTALL_APPDIR})

