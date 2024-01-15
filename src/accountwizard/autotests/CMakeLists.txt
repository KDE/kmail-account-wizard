# SPDX-FileCopyrightText: none
# SPDX-License-Identifier: BSD-3-Clause

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
accountwizard_test(identitybasetest.cpp)
