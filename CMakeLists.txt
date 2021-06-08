set(PIM_VERSION "5.17.3")
cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(kmail-account-wizard VERSION ${PIM_VERSION})


set(LIBRARY_NAMELINK LIBRARY NAMELINK_SKIP)

set(KF5_MIN_VERSION "5.79.0")

find_package(ECM ${KF5_MIN_VERSION} CONFIG REQUIRED)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(ECMInstallIcons)
include(ECMSetupVersion)
include(ECMAddTests)

include(GenerateExportHeader)
include(ECMGenerateHeaders)

include(FeatureSummary)
include(KDEGitCommitHooks)
include(CheckFunctionExists)
include(ECMGeneratePriFile)

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDEFrameworkCompilerSettings NO_POLICY_SCOPE)
include(ECMAddAppIcon)
include(ECMQtDeclareLoggingCategory)

# Do NOT add quote
set(KDEPIM_DEV_VERSION)
set(RELEASE_SERVICE_VERSION "21.04.3")

# add an extra space
if(DEFINED KDEPIM_DEV_VERSION)
    set(KDEPIM_DEV_VERSION " ${KDEPIM_DEV_VERSION}")
endif()

set(KDEPIM_VERSION "${PIM_VERSION}${KDEPIM_DEV_VERSION} (${RELEASE_SERVICE_VERSION})")

set(AKONADI_MIMELIB_VERSION "5.17.2")
set(IDENTITYMANAGEMENT_LIB_VERSION "5.17.2")
set(KLDAP_LIB_VERSION "5.17.2")
set(KMAILTRANSPORT_LIB_VERSION "5.17.2")
set(AKONADI_VERSION "5.17.2")
set(KIMAP_LIB_VERSION "5.17.2")
set(AKONADI_MIMELIB_VERSION "5.17.2")

set(KDEPIM_LIB_VERSION "${PIM_VERSION}")
set(KDEPIM_LIB_SOVERSION "5")

set(QT_REQUIRED_VERSION "5.14.0")
find_package(Qt5 ${QT_REQUIRED_VERSION} CONFIG REQUIRED Widgets Xml DBus Test)
set(PIMCOMMON_LIB_VERSION "5.17.2")
set(LIBKDEPIM_LIB_VERSION "5.17.2")
set(LIBKLEO_LIB_VERSION "5.17.2")

# Find KF5 package
find_package(KF5Wallet ${KF5_MIN_VERSION} CONFIG REQUIRED)
find_package(KF5KCMUtils ${KF5_MIN_VERSION} CONFIG REQUIRED)
find_package(KF5NotifyConfig ${KF5_MIN_VERSION} CONFIG REQUIRED)
find_package(KF5Config ${KF5_MIN_VERSION} CONFIG REQUIRED)
find_package(KF5Service ${KF5_MIN_VERSION} CONFIG REQUIRED)
find_package(KF5DBusAddons ${KF5_MIN_VERSION} CONFIG REQUIRED)
find_package(KF5DocTools ${KF5_MIN_VERSION} REQUIRED)
find_package(KF5TextEditor ${KF5_MIN_VERSION} CONFIG REQUIRED)
find_package(KF5Codecs ${KF5_MIN_VERSION} CONFIG REQUIRED)
find_package(KF5Crash ${KF5_MIN_VERSION} REQUIRED)
find_package(KF5Notifications ${KF5_MIN_VERSION} CONFIG REQUIRED)

find_package(KF5Akonadi ${AKONADI_VERSION} CONFIG REQUIRED)
find_package(KF5IdentityManagement ${IDENTITYMANAGEMENT_LIB_VERSION} CONFIG REQUIRED)
find_package(KF5Ldap ${KLDAP_LIB_VERSION} CONFIG REQUIRED)
find_package(KF5MailTransportAkonadi ${KMAILTRANSPORT_LIB_VERSION} CONFIG REQUIRED)
find_package(KF5PimCommon ${PIMCOMMON_LIB_VERSION} CONFIG REQUIRED)
find_package(KF5Libkdepim ${LIBKDEPIM_LIB_VERSION} CONFIG REQUIRED)
find_package(KF5Libkleo ${LIBKLEO_LIB_VERSION} CONFIG REQUIRED)
find_package(KF5IMAP ${KIMAP_LIB_VERSION} CONFIG REQUIRED)
find_package(MailTransportDBusService CONFIG REQUIRED)
find_package(KF5AkonadiMime ${AKONADI_MIMELIB_VERSION} CONFIG REQUIRED)


include_directories(${kmail-account-wizard_SOURCE_DIR} ${kmail-account-wizard_BINARY_DIR})


# Qt5 package
find_package(Qt5UiTools ${QT_REQUIRED_VERSION} CONFIG REQUIRED)

find_package(KF5Kross ${KF5_MIN_VERSION} CONFIG REQUIRED)
find_package(KF5NewStuff ${KF5_MIN_VERSION} CONFIG REQUIRED)

#it will replace kwallet support
find_package(Qt5Keychain CONFIG)
set_package_properties(Qt5Keychain PROPERTIES
                                   DESCRIPTION "Provides support for secure credentials storage"
                                   URL "https://github.com/frankosterfeld/qtkeychain"
                                   TYPE OPTIONAL)


set( SharedMimeInfo_MINIMUM_VERSION "1.3" )
find_package(SharedMimeInfo ${SharedMimeInfo_MINIMUM_VERSION} REQUIRED)

add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0x050f00)
add_definitions(-DKF_DISABLE_DEPRECATED_BEFORE_AND_AT=0x055000)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(USE_UNITY_CMAKE_SUPPORT "Use UNITY cmake support (speedup compile time)" FALSE)

set(COMPILE_WITH_UNITY_CMAKE_SUPPORT false)
if (USE_UNITY_CMAKE_SUPPORT)
    if(${CMAKE_VERSION} VERSION_LESS "3.16.0")
        message(STATUS "CMAKE version is less than 3.16.0 . We can't use cmake unify build support")
    else()
        set(COMPILE_WITH_UNITY_CMAKE_SUPPORT true)
    endif()
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
