# SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
# SPDX-FileCopyrightText: 2023 Laurent Montel <montel@kde.org>
# SPDX-License-Identifier: BSD-3-Clause

add_library(libaccountwizard SHARED)
target_sources(libaccountwizard PRIVATE
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
    resource.h
    resource.cpp
    transport.h
    transport.cpp
    setupbase.h
    setupbase.cpp
    servertest.h
    servertest.cpp
)

generate_export_header(libaccountwizard BASE_NAME libaccountwizard)

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
    KF6::Crash
    Qt::Xml
    Qt::Quick
    Qt::QuickControls2
    KF6::DBusAddons
    )

set_target_properties(libaccountwizard PROPERTIES OUTPUT_NAME accountwizard VERSION ${PIM_VERSION} SOVERSION ${KDEPIM_LIB_SOVERSION})
install(TARGETS libaccountwizard ${KDE_INSTALL_TARGETS_DEFAULT_ARGS} LIBRARY NAMELINK_SKIP)

if (BUILD_TESTING)
    add_subdirectory(autotests)
endif()

ecm_qt_install_logging_categories(EXPORT ACCOUNTWIZARD FILE accountwizard.categories DESTINATION ${KDE_INSTALL_LOGGINGCATEGORIESDIR})

install(TARGETS libaccountwizard ${KDE_INSTALL_TARGETS_DEFAULT_ARGS} LIBRARY NAMELINK_SKIP)
