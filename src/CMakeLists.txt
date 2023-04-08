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
)

ecm_qt_declare_logging_category(libaccountwizard HEADER accountwizard_debug.h IDENTIFIER ACCOUNTWIZARD_LOG CATEGORY_NAME org.kde.pim.accountwizard
        OLD_CATEGORY_NAMES log_accountwizard
        DESCRIPTION "accountwizard (accountwizard)" EXPORT ACCOUNTWIZARD)


target_link_libraries(libaccountwizard
    KPim${KF_MAJOR_VERSION}::AkonadiCore
    KPim${KF_MAJOR_VERSION}::AkonadiWidgets
    KPim${KF_MAJOR_VERSION}::MailTransport
    KF${KF_MAJOR_VERSION}::KCMUtils
    KF${KF_MAJOR_VERSION}::KIOCore
    KPim${KF_MAJOR_VERSION}::Mime
    KPim${KF_MAJOR_VERSION}::IdentityManagement
    KF${KF_MAJOR_VERSION}::WidgetsAddons
    KPim${KF_MAJOR_VERSION}::Libkdepim
    KF${KF_MAJOR_VERSION}::ItemViews
    KF${KF_MAJOR_VERSION}::I18n
    KPim${KF_MAJOR_VERSION}::Ldap
    KF${KF_MAJOR_VERSION}::Crash
    KPim${KF_MAJOR_VERSION}::Libkleo
    KF${KF_MAJOR_VERSION}::Notifications
    KPim${KF_MAJOR_VERSION}::PimCommon
    KF${KF_MAJOR_VERSION}::XmlGui
    KF${KF_MAJOR_VERSION}::Package
    Qt::Quick
    Qt::QuickControls2
    KF${KF_MAJOR_VERSION}::Codecs
    KF${KF_MAJOR_VERSION}::DBusAddons
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

#add_library(accountwizard_plugin MODULE ${accountwizard_srcs} inprocess-main.cpp)
#if (COMPILE_WITH_UNITY_CMAKE_SUPPORT)
#    set_target_properties(accountwizard_plugin PROPERTIES UNITY_BUILD ON)
#endif()

#target_link_libraries(accountwizard_plugin ${accountwizard_libs})

if ( NOT ACCOUNTWIZARD_NO_GHNS )
    install(FILES accountwizard.knsrc DESTINATION ${KDE_INSTALL_KNSRCDIR})
endif ()
install(TARGETS accountwizard ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
#install(TARGETS accountwizard_plugin DESTINATION ${KDE_INSTALL_PLUGINDIR})
install(PROGRAMS org.kde.accountwizard.desktop DESTINATION ${KDE_INSTALL_APPDIR})
install(FILES accountwizard-mime.xml DESTINATION ${KDE_INSTALL_MIMEDIR})
update_xdg_mimetypes(${KDE_INSTALL_MIMEDIR})

#add_subdirectory(ispdb)
if (BUILD_TESTING)
    add_subdirectory(autotests)
endif()

ecm_qt_install_logging_categories(EXPORT ACCOUNTWIZARD FILE accountwizard.categories DESTINATION ${KDE_INSTALL_LOGGINGCATEGORIESDIR})
