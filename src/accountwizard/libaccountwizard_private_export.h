/*  This file is part of the KDE project
    SPDX-FileCopyrightText: 2023-2024 Laurent Montel <montel.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "libaccountwizard_export.h"

/* Classes which are exported only for unit tests */
#ifdef BUILD_TESTING
#ifndef LIBACCOUNTWIZARD_TESTS_EXPORT
#define LIBACCOUNTWIZARD_TESTS_EXPORT LIBACCOUNTWIZARD_EXPORT
#endif
#else /* not compiling tests */
#define LIBACCOUNTWIZARD_TESTS_EXPORT
#endif
