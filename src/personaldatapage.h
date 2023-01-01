/*
    SPDX-FileCopyrightText: 2009 Volker Krause <vkrause@kde.org>
    SPDX-FileCopyrightText: 2010 Tom Albers <toma@kde.org>
    SPDX-FileCopyrightText: 2012-2023 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "dialog.h"
#include "page.h"
#include "setupmanager.h"

#include "ui_personaldatapage.h"

class Ispdb;

class PersonalDataPage : public Page
{
    Q_OBJECT
public:
    explicit PersonalDataPage(Dialog *parent = nullptr);
    void setHideOptionInternetSearch(bool);

    void leavePageNext() override;
    void leavePageNextRequested() override;

Q_SIGNALS:
    void manualWanted(bool);

private:
    void ispdbSearchFinished(bool ok);
    void slotTextChanged();
    void slotCreateAccountClicked();
    void slotRadioButtonClicked(QAbstractButton *button);
    void slotSearchType(const QString &);
    void automaticConfigureAccount();
    void configureSmtpAccount();
    void configureImapAccount();
    void configurePop3Account();

    Ui::PersonalDataPage ui;
    Ispdb *mIspdb = nullptr;
    SetupManager *const mSetupManager;
};

