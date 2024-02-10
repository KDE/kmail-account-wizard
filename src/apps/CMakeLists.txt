# SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
# SPDX-FileCopyrightText: 2023-2024 Laurent Montel <montel.org>
# SPDX-License-Identifier: BSD-3-Clause


add_executable(accountwizard
    main.cpp
)

qt_add_qml_module(accountwizard
    URI AccountWizard
    QML_FILES
        qml/ConfigurationDelegate.qml
        qml/ConfigurationSelectionPage.qml
        qml/DetailsPage.qml
        qml/Main.qml
        qml/ManualConfigurationPage.qml
        qml/PersonalDataPage.qml
        qml/WizardPage.qml
)

if (COMPILE_WITH_UNITY_CMAKE_SUPPORT)
    set_target_properties(accountwizard PROPERTIES UNITY_BUILD ON)
endif()

target_link_libraries(accountwizard PRIVATE libaccountwizard)

install(TARGETS accountwizard ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
install(PROGRAMS org.kde.accountwizard.desktop DESTINATION ${KDE_INSTALL_APPDIR})
