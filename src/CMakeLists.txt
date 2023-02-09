# SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>

add_subdirectory(packagestructure)

set(accountwizard_srcs
    #cryptopage.cpp
    #dialog.cpp
    wizardmodel.cpp
    controller.cpp
    #typepage.cpp
    #loadpage.cpp
    #global.cpp
    #page.cpp
    key.cpp
    #dynamicpage.cpp
    setupmanager.cpp
    #setuppage.cpp
    resource.cpp
    transport.cpp
    configfile.cpp
    ldap.cpp
    restoreldapsettingsjob.cpp
    identity.cpp
    ispdbhelper.cpp
    setupobject.cpp
    setupautoconfigkolabmail.cpp
    setupautoconfigkolabldap.cpp
    setupautoconfigkolabfreebusy.cpp
    servertest.cpp
    #personaldatapage.cpp
    ispdb/ispdb.cpp
    ispdb/autoconfigkolabmail.cpp
    ispdb/autoconfigkolabldap.cpp
    ispdb/autoconfigkolabfreebusy.cpp
    #cryptopage.h
    #dialog.h
    wizardmodel.h
    controller.h
    #typepage.h
    #loadpage.h
    #global.h
    #page.h
    key.h
    #dynamicpage.h
    setupmanager.h
    #setuppage.h
    resource.h
    transport.h
    configfile.h
    ldap.h
    restoreldapsettingsjob.h
    identity.h
    ispdbhelper.h
    setupobject.h
    setupautoconfigkolabmail.h
    setupautoconfigkolabldap.h
    setupautoconfigkolabfreebusy.h
    servertest.h
    #personaldatapage.h
    ispdb/ispdb.h
    ispdb/autoconfigkolabmail.h
    ispdb/autoconfigkolabldap.h
    ispdb/autoconfigkolabfreebusy.h
    resources.qrc
    )

ecm_qt_declare_logging_category(accountwizard_srcs HEADER accountwizard_debug.h IDENTIFIER ACCOUNTWIZARD_LOG CATEGORY_NAME org.kde.pim.accountwizard
        OLD_CATEGORY_NAMES log_accountwizard
        DESCRIPTION "accountwizard (accountwizard)" EXPORT ACCOUNTWIZARD)


set(accountwizard_libs
    KF5::AkonadiCore
    KF5::AkonadiWidgets
    KF5::MailTransportAkonadi
    KF5::MailTransport
    KF${KF_MAJOR_VERSION}::KCMUtils
    KF${KF_MAJOR_VERSION}::KIOCore
    KF5::Mime
    KF5::IdentityManagement
    KF${KF_MAJOR_VERSION}::WidgetsAddons
    Qt::UiTools
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
    )

#if ( NOT ACCOUNTWIZARD_NO_GHNS )
#    set(accountwizard_srcs ${accountwizard_srcs} providerpage.cpp)
#    #ki18n_wrap_ui(accountwizard_srcs ui/providerpage.ui)
#    set(accountwizard_libs ${accountwizard_libs} KF${KF_MAJOR_VERSION}::NewStuff)
#endif ()

add_executable(accountwizard ${accountwizard_srcs} main.cpp)
if (COMPILE_WITH_UNITY_CMAKE_SUPPORT)
    set_target_properties(accountwizard PROPERTIES UNITY_BUILD ON)
endif()

target_link_libraries(accountwizard ${accountwizard_libs} KF${KF_MAJOR_VERSION}::DBusAddons)

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
