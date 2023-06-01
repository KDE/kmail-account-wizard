# SPDX-FileCopyrightText: none
# SPDX-License-Identifier: BSD-3-Clause

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/.. ${accountwizard_BINARY_DIR} )
add_definitions (-DTEST_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data")
add_definitions (-DCURRENT_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}")

add_executable(ldaptest ldaptest.cpp ../ldap.cpp ../setupobject.cpp ../restoreldapsettingsjob.cpp)
add_test(NAME ldaptest COMMAND ldaptest)
ecm_mark_as_test(ldaptest)

target_link_libraries(ldaptest
    KF6::KIOCore
    KPim6::Mime
    KPim6::LdapCore
    KPim6::LdapWidgets
    Qt::Test
    KF6::I18n
    KF6::ConfigCore
    Qt::Widgets
    )
