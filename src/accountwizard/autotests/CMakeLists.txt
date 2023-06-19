# SPDX-FileCopyrightText: none
# SPDX-License-Identifier: BSD-3-Clause

#include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/.. ${accountwizard_BINARY_DIR} )
#add_definitions (-DTEST_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data")
#add_definitions (-DCURRENT_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}")

#add_executable(ldaptest ldaptest.cpp ../ldap.cpp ../setupobject.cpp ../restoreldapsettingsjob.cpp)
#add_test(NAME ldaptest COMMAND ldaptest)
#ecm_mark_as_test(ldaptest)

#target_link_libraries(ldaptest
#    KF6::KIOCore
#    KPim6::Mime
#    KPim6::LdapCore
#    KPim6::LdapWidgets
#    Qt::Test
#    KF6::I18n
#    KF6::ConfigCore
#    Qt::Widgets
#    )

macro(accountwizard_test _source)
    set(_test ${_source})
    get_filename_component(_name ${_source} NAME_WE )
    add_executable( ${_name} ${_test} ${ARGN} ${_name}.h)
    add_test(NAME ${_name} COMMAND ${_name} )
    ecm_mark_as_test(${_name})
    target_link_libraries( ${_name} Qt::Test libaccountwizard)
    set_target_properties(${_name} PROPERTIES
        DISABLE_PRECOMPILE_HEADERS ON
    )

endmacro()

accountwizard_test(manualconfigurationtest.cpp)
accountwizard_test(setupmanagertest.cpp)
# TODO reactivate it accountwizard_test(resourcetest.cpp)
