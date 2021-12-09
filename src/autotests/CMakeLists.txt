# SPDX-FileCopyrightText: none
# SPDX-License-Identifier: BSD-3-Clause

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/.. ${accountwizard_BINARY_DIR} )
add_definitions (-DTEST_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data")
add_definitions (-DCURRENT_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}")

add_executable(ldaptest ldaptest.cpp ../ldap.cpp ../setupobject.cpp ../restoreldapsettingsjob.cpp)
add_test(NAME ldaptest COMMAND ldaptest)
ecm_mark_as_test(ldaptest)

target_link_libraries(ldaptest
    KF5::KIOCore
    KF5::Mime
    KF5::Ldap
    Qt::Test
    KF5::Libkdepim
    KF5::I18n
    KF5::ConfigCore
    Qt::Widgets
    )
