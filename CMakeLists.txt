# SPDX-FileCopyrightText: none
# SPDX-License-Identifier: BSD-3-Clause

set(PIM_VERSION "5.23.40")
cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
project(kmail-account-wizard VERSION ${PIM_VERSION})

set(KF_MIN_VERSION "5.105.0")

find_package(ECM ${KF_MIN_VERSION} CONFIG REQUIRED)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(ECMInstallIcons)
include(ECMSetupVersion)
include(ECMAddTests)

include(GenerateExportHeader)
include(ECMGenerateHeaders)

include(FeatureSummary)
include(KDEGitCommitHooks)
include(KDEClangFormat)
file(GLOB_RECURSE ALL_CLANG_FORMAT_SOURCE_FILES *.cpp *.h *.c)
kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})
include(CheckFunctionExists)

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(ECMAddAppIcon)
include(ECMQtDeclareLoggingCategory)
include(ECMDeprecationSettings)

# Do NOT add quote
set(KDEPIM_DEV_VERSION alpha)
set(RELEASE_SERVICE_VERSION "23.07.40")
if (QT_MAJOR_VERSION STREQUAL "6")
    set(QT_REQUIRED_VERSION "6.4.0")
    set(KF_MIN_VERSION "5.240.0")
    set(KF_MAJOR_VERSION "6")
else()
    set(KF_MAJOR_VERSION "5")
endif()
# add an extra space
if(DEFINED KDEPIM_DEV_VERSION)
    set(KDEPIM_DEV_VERSION " ${KDEPIM_DEV_VERSION}")
endif()

set(KDEPIM_VERSION "${PIM_VERSION}${KDEPIM_DEV_VERSION} (${RELEASE_SERVICE_VERSION})")

set(AKONADI_MIMELIB_VERSION "5.23.40")
set(IDENTITYMANAGEMENT_LIB_VERSION "5.23.40")
set(KLDAP_LIB_VERSION "5.23.40")
set(KMAILTRANSPORT_LIB_VERSION "5.23.40")
set(AKONADI_VERSION "5.23.40")
set(KIMAP_LIB_VERSION "5.23.40")
set(AKONADI_MIMELIB_VERSION "5.23.40")
set(KMIME_VERSION "5.23.40")

set(KDEPIM_LIB_VERSION "${PIM_VERSION}")
set(KDEPIM_LIB_SOVERSION "5")

set(QT_REQUIRED_VERSION "5.15.2")
if (QT_MAJOR_VERSION STREQUAL "6")
    set(QT_REQUIRED_VERSION "6.4.0")
endif()
find_package(Qt5 ${QT_REQUIRED_VERSION} CONFIG REQUIRED Widgets Xml DBus Test)
set(PIMCOMMON_LIB_VERSION "5.23.40")
set(LIBKDEPIM_LIB_VERSION "5.23.40")
set(LIBKLEO_LIB_VERSION "5.23.40")

# Find KF5 package
find_package(KF5Wallet ${KF_MIN_VERSION} CONFIG REQUIRED)
find_package(KF5KCMUtils ${KF_MIN_VERSION} CONFIG REQUIRED)
find_package(KF5NotifyConfig ${KF_MIN_VERSION} CONFIG REQUIRED)
find_package(KF5Config ${KF_MIN_VERSION} CONFIG REQUIRED)
find_package(KF5Service ${KF_MIN_VERSION} CONFIG REQUIRED)
find_package(KF5DBusAddons ${KF_MIN_VERSION} CONFIG REQUIRED)
find_package(KF5DocTools ${KF_MIN_VERSION} REQUIRED)
find_package(KF5TextEditor ${KF_MIN_VERSION} CONFIG REQUIRED)
find_package(KF5Codecs ${KF_MIN_VERSION} CONFIG REQUIRED)
find_package(KF5Crash ${KF_MIN_VERSION} REQUIRED)
find_package(KF5Notifications ${KF_MIN_VERSION} CONFIG REQUIRED)

find_package(KPim${KF_MAJOR_VERSION}Akonadi ${AKONADI_VERSION} CONFIG REQUIRED)
find_package(KPim${KF_MAJOR_VERSION}IdentityManagement ${IDENTITYMANAGEMENT_LIB_VERSION} CONFIG REQUIRED)
find_package(KPim${KF_MAJOR_VERSION}Ldap ${KLDAP_LIB_VERSION} CONFIG REQUIRED)
find_package(KPim${KF_MAJOR_VERSION}MailTransportAkonadi ${KMAILTRANSPORT_LIB_VERSION} CONFIG REQUIRED)
find_package(KPim${KF_MAJOR_VERSION}MailTransport ${KMAILTRANSPORT_LIB_VERSION} CONFIG REQUIRED)
find_package(KPim${KF_MAJOR_VERSION}PimCommon ${PIMCOMMON_LIB_VERSION} CONFIG REQUIRED)
find_package(KPim${KF_MAJOR_VERSION}Libkdepim ${LIBKDEPIM_LIB_VERSION} CONFIG REQUIRED)
find_package(KPim${KF_MAJOR_VERSION}Libkleo ${LIBKLEO_LIB_VERSION} CONFIG REQUIRED)
find_package(KPim${KF_MAJOR_VERSION}IMAP ${KIMAP_LIB_VERSION} CONFIG REQUIRED)
find_package(KPim${KF_MAJOR_VERSION}MailTransportDBusService CONFIG REQUIRED)
find_package(KPim${KF_MAJOR_VERSION}AkonadiMime ${AKONADI_MIMELIB_VERSION} CONFIG REQUIRED)
find_package(KPim${KF_MAJOR_VERSION}Mime ${KMIME_VERSION} CONFIG REQUIRED)


include_directories(${kmail-account-wizard_SOURCE_DIR} ${kmail-account-wizard_BINARY_DIR})


# Qt5 package
find_package(Qt5UiTools ${QT_REQUIRED_VERSION} CONFIG REQUIRED)

find_package(KF5Kross ${KF_MIN_VERSION} CONFIG REQUIRED)
find_package(KF5NewStuff ${KF_MIN_VERSION} CONFIG REQUIRED)

#it will replace kwallet support
find_package(Qt5Keychain CONFIG)
set_package_properties(Qt5Keychain PROPERTIES
                                   DESCRIPTION "Provides support for secure credentials storage"
                                   URL "https://github.com/frankosterfeld/qtkeychain"
                                   TYPE OPTIONAL)


set( SharedMimeInfo_MINIMUM_VERSION "2.0" )
find_package(SharedMimeInfo ${SharedMimeInfo_MINIMUM_VERSION} REQUIRED)

ecm_set_disabled_deprecation_versions(QT 5.15.2  KF 5.105.0)


option(USE_UNITY_CMAKE_SUPPORT "Use UNITY cmake support (speedup compile time)" OFF)

set(COMPILE_WITH_UNITY_CMAKE_SUPPORT OFF)
if (USE_UNITY_CMAKE_SUPPORT)
    set(COMPILE_WITH_UNITY_CMAKE_SUPPORT ON)
endif()
# allow to disable GHNS support on resource-constrained systems
option( ACCOUNTWIZARD_NO_GHNS "Disable GHNS support in the account wizard" FALSE )

option(KDEPIM_ENTERPRISE_BUILD "Enable features specific to the enterprise branch, which are normally disabled. Also, it disables many components not needed for Kontact such as the Kolab client." FALSE)

# config-enterprise.h is needed for both ENTERPRISE_BUILD and BUILD_EVERYTHING
configure_file(src/config-enterprise.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/src/config-enterprise.h )

if ( ACCOUNTWIZARD_NO_GHNS )
    add_definitions( -DACCOUNTWIZARD_NO_GHNS )
endif ()



add_subdirectory(src)


kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)
ki18n_install(po)
feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
