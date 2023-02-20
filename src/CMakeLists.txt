# SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
# SPDX-FileCopyrightText: 2023 Laurent Montel <montel@kde.org>
# SPDX-License-Identifier: BSD-3-Clause
add_subdirectory(packagestructure)

add_library(libaccountwizard)

generate_export_header(libaccountwizard BASE_NAME libaccountwizard)

target_sources(libaccountwizard PRIVATE
    #cryptopage.cpp
    #dialog.cpp
    wizardmodel.cpp
    controller.cpp
    #global.cpp
    key.cpp
    setupmanager.cpp
    resource.cpp
    transport.cpp
    configfile.cpp
    ldap.cpp
    restoreldapsettingsjob.cpp
    identity.cpp
    setupobject.cpp
    setupispdb.cpp
    setupautoconfigkolabmail.cpp
    setupautoconfigkolabldap.cpp
    setupautoconfigkolabfreebusy.cpp
    servertest.cpp
    ispdb.cpp
    ispdb/autoconfigkolabmail.cpp
    ispdb/autoconfigkolabldap.cpp
    ispdb/autoconfigkolabfreebusy.cpp
    ispdb/autoconfigkolabfreebusy.h

    wizardmodel.h
    controller.h
    key.h
    setupmanager.h
    resource.h
    transport.h
    configfile.h
    ldap.h
    restoreldapsettingsjob.h
    identity.h
    setupobject.h
    setupautoconfigkolabmail.h
    setupautoconfigkolabldap.h
    setupautoconfigkolabfreebusy.h
    servertest.h
    ispdb.h
    ispdb/autoconfigkolabmail.h
    ispdb/autoconfigkolabldap.h
    ispdb/autoconfigkolabfreebusy.h
    resources.qrc
    )

ecm_qt_declare_logging_category(libaccountwizard HEADER accountwizard_debug.h IDENTIFIER ACCOUNTWIZARD_LOG CATEGORY_NAME org.kde.pim.accountwizard
        OLD_CATEGORY_NAMES log_accountwizard
        DESCRIPTION "accountwizard (accountwizard)" EXPORT ACCOUNTWIZARD)


target_link_libraries(libaccountwizard
    KF5::AkonadiCore
    KF5::AkonadiWidgets
    KF5::MailTransportAkonadi
    KF5::MailTransport
    KF${KF_MAJOR_VERSION}::KCMUtils
    KF${KF_MAJOR_VERSION}::KIOCore
    KF5::Mime
    KF5::IdentityManagement
    KF${KF_MAJOR_VERSION}::WidgetsAddons
    KF5::Libkdepim
    KF${KF_MAJOR_VERSION}::ItemViews
    KF${KF_MAJOR_VERSION}::I18n
    KF5::Ldap
    KF${KF_MAJOR_VERSION}::Crash
    KF5::Libkleo
    KF${KF_MAJOR_VERSION}::Notifications
    KF5::PimCommon
    KF${KF_MAJOR_VERSION}::XmlGui
    KF${KF_MAJOR_VERSION}::Package
    Qt::Quick
    Qt::QuickControls2
    KF${KF_MAJOR_VERSION}::Codecs
    KF${KF_MAJOR_VERSION}::DBusAddons
    )

add_executable(accountwizard ${accountwizard_srcs} main.cpp)
if (COMPILE_WITH_UNITY_CMAKE_SUPPORT)
    set_target_properties(accountwizard PROPERTIES UNITY_BUILD ON)
endif()

target_link_libraries(accountwizard libaccountwizard)

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

add_subdirectory(ispdb)
if (BUILD_TESTING)
    add_subdirectory(autotests)
endif()

ecm_qt_install_logging_categories(EXPORT ACCOUNTWIZARD FILE accountwizard.categories DESTINATION ${KDE_INSTALL_LOGGINGCATEGORIESDIR})
