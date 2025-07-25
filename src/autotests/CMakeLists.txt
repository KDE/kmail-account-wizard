# SPDX-FileCopyrightText: none
# SPDX-License-Identifier: BSD-3-Clause

include_directories(..)

macro(accountwizard_test _source)
    set(_test ${_source})
    get_filename_component(_name ${_source} NAME_WE)
    add_executable(
        ${_name}
        ${_test}
        ${ARGN}
        ${_name}.h
    )
    add_test(NAME ${_name} COMMAND ${_name})
    ecm_mark_as_test(${_name})
    target_link_libraries(
        ${_name}
        Qt::Test
        accountwizard_static
    )
    set_target_properties(
        ${_name}
        PROPERTIES
            DISABLE_PRECOMPILE_HEADERS
                ON
    )
endmacro()

if(NOT DEFINED ENV{KDECI_BUILD})
    add_akonadi_isolated_test(accountconfigurationtest.cpp LINK_LIBRARIES accountwizard_static)
endif()
accountwizard_test(setupmanagertest.cpp)
