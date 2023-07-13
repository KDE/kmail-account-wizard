# SPDX-FileCopyrightText: none
# SPDX-License-Identifier: BSD-3-Clause

set(PIM_VERSION "5.240.40")
cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
project(kmail-account-wizard VERSION ${PIM_VERSION})

set(KF_MIN_VERSION "5.240.0")

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
include(ECMQmlModule)

# Do NOT add quote
set(KDEPIM_DEV_VERSION alpha)
set(RELEASE_SERVICE_VERSION "23.07.40")
# add an extra space
if(DEFINED KDEPIM_DEV_VERSION)
    set(KDEPIM_DEV_VERSION " ${KDEPIM_DEV_VERSION}")
endif()

set(KDEPIM_VERSION "${PIM_VERSION}${KDEPIM_DEV_VERSION} (${RELEASE_SERVICE_VERSION})")

set(AKONADI_MIMELIB_VERSION "5.240.40")
set(IDENTITYMANAGEMENT_LIB_VERSION "5.240.40")
set(KLDAP_LIB_VERSION "5.240.40")
set(KMAILTRANSPORT_LIB_VERSION "5.240.40")
set(AKONADI_VERSION "5.240.40")
set(KIMAP_LIB_VERSION "5.240.40")
set(AKONADI_MIMELIB_VERSION "5.240.40")

set(KDEPIM_LIB_VERSION "${PIM_VERSION}")
set(QT_REQUIRED_VERSION "6.4.0")
find_package(Qt6 ${QT_REQUIRED_VERSION} CONFIG REQUIRED Widgets DBus Test Quick QuickControls2)
set(PIMCOMMON_LIB_VERSION "5.240.40")
set(LIBKLEO_LIB_VERSION "5.240.40")
set(KPIM_MIME_VERSION "5.240.40")

# Find KF6 package
find_package(KF6DBusAddons ${KF5_MIN_VERSION} CONFIG REQUIRED)
find_package(KF6DocTools ${KF5_MIN_VERSION} REQUIRED)
find_package(KF6Crash ${KF5_MIN_VERSION} REQUIRED)
find_package(KF6Package ${KF5_MIN_VERSION} CONFIG REQUIRED)

find_package(KPim6Mime ${KPIM_MIME_VERSION} CONFIG REQUIRED)
find_package(KPim6Akonadi ${AKONADI_VERSION} CONFIG REQUIRED)
find_package(KPim6IdentityManagementCore ${IDENTITYMANAGEMENT_LIB_VERSION} CONFIG REQUIRED)
find_package(KPim6LdapCore ${KLDAP_LIB_VERSION} CONFIG REQUIRED)
find_package(KPim6LdapWidgets ${KLDAP_LIB_VERSION} CONFIG REQUIRED)
find_package(KPim6MailTransport ${KMAILTRANSPORT_LIB_VERSION} CONFIG REQUIRED)
find_package(KPim6Libkleo ${LIBKLEO_LIB_VERSION} CONFIG REQUIRED)
find_package(KPim6IMAP ${KIMAP_LIB_VERSION} CONFIG REQUIRED)
find_package(KPim6MailTransportDBusService CONFIG REQUIRED)
find_package(KPim6AkonadiMime ${AKONADI_MIMELIB_VERSION} CONFIG REQUIRED)

include_directories(${kmail-account-wizard_SOURCE_DIR} ${kmail-account-wizard_BINARY_DIR})


find_package(Qt6Keychain CONFIG)
set_package_properties(Qt6Keychain PROPERTIES
                                   DESCRIPTION "Provides support for secure credentials storage"
                                   URL "https://github.com/frankosterfeld/qtkeychain"
                                   TYPE OPTIONAL)


ecm_set_disabled_deprecation_versions(QT 6.5  KF 5.240.0)

option(USE_UNITY_CMAKE_SUPPORT "Use UNITY cmake support (speedup compile time)" OFF)

set(COMPILE_WITH_UNITY_CMAKE_SUPPORT OFF)
if (USE_UNITY_CMAKE_SUPPORT)
    set(COMPILE_WITH_UNITY_CMAKE_SUPPORT ON)
endif()
if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    add_definitions(-DQT_QML_DEBUG)
endif()

add_subdirectory(src)
add_subdirectory(components)

kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)
ki18n_install(po)
feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
