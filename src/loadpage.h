/*
    SPDX-FileCopyrightText: 2009 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "page.h"

#include "ui_loadpage.h"

namespace Kross
{
class Action;
}

class LoadPage : public Page
{
    Q_OBJECT
public:
    explicit LoadPage(KAssistantDialog *parent);

    void enterPageNext() override;
    void enterPageBack() override;

    void exportObject(QObject *object, const QString &name);

Q_SIGNALS:
    void aboutToStart();

private:
    Ui::LoadPage ui;
    QVector<QPair<QObject *, QString>> m_exportedObjects;
    Kross::Action *m_action = nullptr;
};

