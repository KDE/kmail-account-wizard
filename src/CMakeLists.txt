# SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
# SPDX-FileCopyrightText: 2023 Laurent Montel <montel@kde.org>
# SPDX-License-Identifier: BSD-3-Clause

add_library(libaccountwizard STATIC
    identity.h
    identity.cpp
    setupmanager.h
    setupmanager.cpp

    ispdb/configurationmodel.h
    ispdb/configurationmodel.cpp
    ispdb/serverconfiguration.h
    ispdb/serverconfiguration.cpp
    ispdbservice.h
    ispdbservice.cpp
    manualconfiguration.h
    manualconfiguration.cpp
)

ecm_qt_declare_logging_category(libaccountwizard HEADER accountwizard_debug.h IDENTIFIER ACCOUNTWIZARD_LOG CATEGORY_NAME org.kde.pim.accountwizard
        OLD_CATEGORY_NAMES log_accountwizard
        DESCRIPTION "accountwizard (accountwizard)" EXPORT ACCOUNTWIZARD)


target_link_libraries(libaccountwizard
    KPim6::AkonadiCore
    KPim6::AkonadiWidgets
    KPim6::MailTransport
    KPim6::Mime
    KF6::KIOCore
    KPim6::IdentityManagementCore
    KF6::WidgetsAddons
    KF6::I18n
    KPim6::LdapCore
    KF6::Crash
    KPim6::Libkleo
    KF6::Package
    Qt::Xml
    Qt::Quick
    Qt::QuickControls2
    KF6::DBusAddons
    )

add_executable(accountwizard
    main.cpp
    resources.qrc
)

if (COMPILE_WITH_UNITY_CMAKE_SUPPORT)
    set_target_properties(accountwizard PROPERTIES UNITY_BUILD ON)
endif()

target_link_libraries(accountwizard PRIVATE libaccountwizard)

install(TARGETS libaccountwizard ${KDE_INSTALL_TARGETS_DEFAULT_ARGS} LIBRARY NAMELINK_SKIP)

if ( NOT ACCOUNTWIZARD_NO_GHNS )
    install(FILES accountwizard.knsrc DESTINATION ${KDE_INSTALL_KNSRCDIR})
endif ()
install(TARGETS accountwizard ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
install(PROGRAMS org.kde.accountwizard.desktop DESTINATION ${KDE_INSTALL_APPDIR})

if (BUILD_TESTING)
    add_subdirectory(autotests)
endif()

ecm_qt_install_logging_categories(EXPORT ACCOUNTWIZARD FILE accountwizard.categories DESTINATION ${KDE_INSTALL_LOGGINGCATEGORIESDIR})
