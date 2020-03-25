set(KDEPIM_VERSION_NUMBER "5.14.40")
cmake_minimum_required(VERSION 3.5)
project(kmail-account-wizard VERSION ${KDEPIM_VERSION_NUMBER})


set(LIBRARY_NAMELINK LIBRARY NAMELINK_SKIP)

set(KF5_MIN_VERSION "5.68.0")

find_package(ECM ${KF5_MIN_VERSION} CONFIG REQUIRED)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(ECMInstallIcons)
include(ECMSetupVersion)
include(ECMAddTests)

include(GenerateExportHeader)
include(ECMGenerateHeaders)

include(FeatureSummary)
include(CheckFunctionExists)
include(ECMGeneratePriFile)

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDEFrameworkCompilerSettings NO_POLICY_SCOPE)
include(ECMAddAppIcon)
include(ECMQtDeclareLoggingCategory)

# Do NOT add quote
set(KDEPIM_DEV_VERSION alpha)
set(RELEASE_SERVICE_VERSION "20.03.80")

# add an extra space
if(DEFINED KDEPIM_DEV_VERSION)
    set(KDEPIM_DEV_VERSION " ${KDEPIM_DEV_VERSION}")
endif()

set(KDEPIM_VERSION "${KDEPIM_VERSION_NUMBER}${KDEPIM_DEV_VERSION} (${RELEASE_SERVICE_VERSION})")

set(AKONADI_MIMELIB_VERSION "5.13.80")
set(IDENTITYMANAGEMENT_LIB_VERSION "5.13.80")
set(KLDAP_LIB_VERSION "5.13.80")
set(KMAILTRANSPORT_LIB_VERSION "5.13.80")
set(AKONADI_VERSION "5.13.80")
set(KIMAP_LIB_VERSION "5.13.80")
set(AKONADI_MIMELIB_VERSION "5.13.80")

set(KDEPIM_LIB_VERSION "${KDEPIM_VERSION_NUMBER}")
set(KDEPIM_LIB_SOVERSION "5")

set(QT_REQUIRED_VERSION "5.12.0")
find_package(Qt5 ${QT_REQUIRED_VERSION} CONFIG REQUIRED Widgets Xml DBus Test)
set(PIMCOMMON_LIB_VERSION_LIB "5.13.80")
set(LIBKDEPIM_LIB_VERSION_LIB "5.13.80")
set(LIBKLEO_LIB_VERSION_LIB "5.13.80")

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
find_package(KF5PimCommon ${PIMCOMMON_LIB_VERSION_LIB} CONFIG REQUIRED)
find_package(KF5LibkdepimAkonadi ${LIBKDEPIM_LIB_VERSION_LIB} CONFIG REQUIRED)
find_package(KF5Libkleo ${LIBKLEO_LIB_VERSION_LIB} CONFIG REQUIRED)
find_package(KF5IMAP ${KIMAP_LIB_VERSION} CONFIG REQUIRED)
find_package(MailTransportDBusService CONFIG REQUIRED)
find_package(KF5AkonadiMime ${AKONADI_MIMELIB_VERSION} CONFIG REQUIRED)


include_directories(${kmail-account-wizard_SOURCE_DIR} ${kmail-account-wizard_BINARY_DIR})


# Qt5 package
find_package(Qt5UiTools ${QT_REQUIRED_VERSION} CONFIG REQUIRED)

find_package(KF5Kross ${KF5_MIN_VERSION} CONFIG REQUIRED)
find_package(KF5NewStuff ${KF5_MIN_VERSION} CONFIG REQUIRED)

set( SharedMimeInfo_MINIMUM_VERSION "1.3" )
find_package(SharedMimeInfo ${SharedMimeInfo_MINIMUM_VERSION} REQUIRED)

if (EXISTS "${CMAKE_SOURCE_DIR}/.git")
   add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0x050e00)
   add_definitions(-DKF_DISABLE_DEPRECATED_BEFORE_AND_AT=0x054400)
endif()


# allow to disable GHNS support on resource-constrained systems
option( ACCOUNTWIZARD_NO_GHNS "Disable GHNS support in the account wizard" FALSE )

option(KDEPIM_ENTERPRISE_BUILD "Enable features specific to the enterprise branch, which are normally disabled. Also, it disables many components not needed for Kontact such as the Kolab client." FALSE)

option( ACCOUNTWIZARD_NO_REGENERATE_MIME "Don't regenerate mime file (only for developper)" FALSE )

# config-enterprise.h is needed for both ENTERPRISE_BUILD and BUILD_EVERYTHING
configure_file(src/config-enterprise.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/src/config-enterprise.h )

if ( ACCOUNTWIZARD_NO_GHNS )
    add_definitions( -DACCOUNTWIZARD_NO_GHNS )
endif ()



add_subdirectory(src)


feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
