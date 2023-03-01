# SPDX-FileCopyrightText: none
# SPDX-License-Identifier: BSD-3-Clause

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/.. ${accountwizard_BINARY_DIR} )
add_definitions (-DTEST_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data")
add_definitions (-DCURRENT_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}")

add_executable(ldaptest ldaptest.cpp ../ldap.cpp ../setupobject.cpp ../restoreldapsettingsjob.cpp)
add_test(NAME ldaptest COMMAND ldaptest)
ecm_mark_as_test(ldaptest)

target_link_libraries(ldaptest
    KF${KF_MAJOR_VERSION}::KIOCore
    KPim${KF_MAJOR_VERSION}::Mime
    KPim${KF_MAJOR_VERSION}::Ldap
    Qt::Test
    KPim${KF_MAJOR_VERSION}::Libkdepim
    KF${KF_MAJOR_VERSION}::I18n
    KF${KF_MAJOR_VERSION}::ConfigCore
    Qt::Widgets
    )
