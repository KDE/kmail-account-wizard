/*
    SPDX-FileCopyrightText: 2009 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "page.h"
#include "ui_setuppage.h"

class QStandardItemModel;

class SetupPage : public Page
{
    Q_OBJECT
public:
    explicit SetupPage(KAssistantDialog *parent);
    void enterPageNext() override;

    enum MessageType { Success, Info, Error };
    void addMessage(MessageType type, const QString &msg);
    void setStatus(const QString &msg);
    void setProgress(int percent);

private:
    void detailsClicked();
    Ui::SetupPage ui;
    QStandardItemModel *m_msgModel = nullptr;
};

