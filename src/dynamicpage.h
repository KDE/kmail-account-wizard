/*
    SPDX-FileCopyrightText: 2009 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef DYNAMICPAGE_H
#define DYNAMICPAGE_H

#include "page.h"

class DynamicPage : public Page
{
    Q_OBJECT
public:
    explicit DynamicPage(const QString &uiFile, KAssistantDialog *parent = nullptr);

public Q_SLOTS:
    /// returns the top-level widget of the UI file
    Q_SCRIPTABLE QObject *widget() const;

private:
    QWidget *m_dynamicWidget = nullptr;
};

#endif
