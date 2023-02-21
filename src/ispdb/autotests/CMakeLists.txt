
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/.. )
add_definitions (-DAUTOCONFIG_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data")
add_definitions (-DCURRENT_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}")

set(ispdbtest_srcs
  ../ispdb.cpp
  ../../accountwizard_debug.cpp
)

set(ispdbtest_libs
  KF${KF_MAJOR_VERSION}::KIOCore
  KPim${KF_MAJOR_VERSION}::Mime
  Qt::Test
  Qt::Xml
  Qt::Network
  KF${KF_MAJOR_VERSION}::I18n
)

add_executable(ispdbtest ispdbtest.cpp ${ispdbtest_srcs})
add_test(NAME ispdbtest COMMAND ispdbtest)
ecm_mark_as_test(ispdbtest)
ecm_mark_nongui_executable(ispdbtest)

target_link_libraries(ispdbtest
  ${ispdbtest_libs}
)

#TODO: Needs to run errorserver.py

set(autoconfigkolabtest_srcs
  ${ispdbtest_srcs}
  ../autoconfigkolabmail.cpp
)

add_executable(autoconfigkolabmailtest autoconfigkolabmailtest.cpp ${autoconfigkolabtest_srcs})
add_test(NAME autoconfigkolabmailtest COMMAND autoconfigkolabmailtest)
ecm_mark_as_test(autoconfigkolabmailtest)
ecm_mark_nongui_executable(autoconfigkolabmailtest)

target_link_libraries(autoconfigkolabmailtest
  ${ispdbtest_libs}
)

add_executable(autoconfigkolabldaptest autoconfigkolabldaptest.cpp ../autoconfigkolabldap.cpp ${autoconfigkolabtest_srcs})
add_test(NAME autoconfigkolabldaptest COMMAND autoconfigkolabldaptest)
ecm_mark_as_test(autoconfigkolabldaptest)
ecm_mark_nongui_executable(autoconfigkolabldaptest)

target_link_libraries(autoconfigkolabldaptest
  KF5::Ldap
  ${ispdbtest_libs}
)

add_executable(autoconfigkolabfreebusytest autoconfigkolabfreebusy.cpp ../autoconfigkolabfreebusy.cpp ${autoconfigkolabtest_srcs})
add_test(NAME autoconfigkolabfreebusytest COMMAND autoconfigkolabfreebusytest)
ecm_mark_as_test(autoconfigkolabfreebusytest)
ecm_mark_nongui_executable(autoconfigkolabfreebusytest)

target_link_libraries(autoconfigkolabfreebusytest
  ${ispdbtest_libs}
)
