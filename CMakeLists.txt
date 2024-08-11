# SPDX-FileCopyrightText: none
# SPDX-License-Identifier: BSD-3-Clause

set(PIM_VERSION "6.2.40")
cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
project(kmail-account-wizard VERSION ${PIM_VERSION})
set(KDEPIM_LIB_SOVERSION "6")

set(KF_MIN_VERSION "6.4.0")

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
include(ECMFeatureSummary)
include(ECMQmlModule)

# Do NOT add quote
set(KDEPIM_DEV_VERSION alpha)
set(RELEASE_SERVICE_VERSION "24.11.40")
# add an extra space
if(DEFINED KDEPIM_DEV_VERSION)
    set(KDEPIM_DEV_VERSION " ${KDEPIM_DEV_VERSION}")
endif()

set(KDEPIM_VERSION "${PIM_VERSION}${KDEPIM_DEV_VERSION} (${RELEASE_SERVICE_VERSION})")

set(AKONADI_MIMELIB_VERSION "6.2.40")
set(IDENTITYMANAGEMENT_LIB_VERSION "6.2.40")
set(KMAILTRANSPORT_LIB_VERSION "6.2.40")
set(AKONADI_VERSION "6.2.40")

set(KDEPIM_LIB_VERSION "${PIM_VERSION}")
set(QT_REQUIRED_VERSION "6.7.0")
find_package(Qt6 ${QT_REQUIRED_VERSION} CONFIG REQUIRED Core Widgets DBus Test Quick QuickControls2)
set(PIMCOMMON_LIB_VERSION "6.2.40")
set(KPIM_MIME_VERSION "6.2.40")
set(QT6KEYCHAIN_LIB_VERSION "0.14.2")

# Find KF6 package
find_package(KF6I18n ${KF_MIN_VERSION} CONFIG REQUIRED)
find_package(KF6DBusAddons ${KF_MIN_VERSION} CONFIG REQUIRED)
find_package(KF6DocTools ${KF_MIN_VERSION} REQUIRED)
find_package(KF6Crash ${KF_MIN_VERSION} REQUIRED)
find_package(KF6KIO ${KF_MIN_VERSION} REQUIRED)
find_package(KF6IconThemes ${KF_MIN_VERSION} QUIET)
find_package(KPim6Mime ${KPIM_MIME_VERSION} CONFIG REQUIRED)
find_package(KPim6Akonadi ${AKONADI_VERSION} CONFIG REQUIRED)
find_package(KPim6IdentityManagementCore ${IDENTITYMANAGEMENT_LIB_VERSION} CONFIG REQUIRED)
find_package(KPim6MailTransport ${KMAILTRANSPORT_LIB_VERSION} CONFIG REQUIRED)

include_directories(${kmail-account-wizard_SOURCE_DIR} ${kmail-account-wizard_BINARY_DIR})

qt_policy(SET QTP0001 NEW)


find_package(Qt6Keychain ${QT6KEYCHAIN_LIB_VERSION} CONFIG)
set_package_properties(Qt6Keychain PROPERTIES
                                   DESCRIPTION "Provides support for secure credentials storage"
                                   URL "https://github.com/frankosterfeld/qtkeychain"
                                   TYPE OPTIONAL)

add_definitions(-DQT_NO_CONTEXTLESS_CONNECT)
ecm_set_disabled_deprecation_versions(QT 6.7.2 KF 6.5.0)

option(USE_UNITY_CMAKE_SUPPORT "Use UNITY cmake support (speedup compile time)" OFF)

set(COMPILE_WITH_UNITY_CMAKE_SUPPORT OFF)
if (USE_UNITY_CMAKE_SUPPORT)
    set(COMPILE_WITH_UNITY_CMAKE_SUPPORT ON)
endif()
if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    add_definitions(-DQT_QML_DEBUG)
endif()

add_subdirectory(src)

kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)
ki18n_install(po)
ecm_feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
