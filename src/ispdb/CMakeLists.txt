# SPDX-FileCopyrightText: none
# SPDX-License-Identifier: BSD-3-Clause

set(ispdb_srcs
    main.cpp
    )
ecm_qt_declare_logging_category(ispdb_srcs HEADER accountwizard_debug.h IDENTIFIER ACCOUNTWIZARD_LOG CATEGORY_NAME org.kde.pim.accountwizard)
add_executable(ispdb ${ispdb_srcs})

target_link_libraries(ispdb
    libaccountwizard
    KF${KF_MAJOR_VERSION}::I18n
    KF${KF_MAJOR_VERSION}::KIOCore
    KPim${KF_MAJOR_VERSION}::Mime
    Qt::Widgets
    )

install(TARGETS ispdb ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
if (BUILD_TESTING)
    # theses tests didn't work on CI from long time => disable them
    #add_subdirectory(autotests)
endif()
