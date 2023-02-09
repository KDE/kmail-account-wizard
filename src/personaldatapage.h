/*
    SPDX-FileCopyrightText: 2009 Volker Krause <vkrause@kde.org>
    SPDX-FileCopyrightText: 2010 Tom Albers <toma@kde.org>
    SPDX-FileCopyrightText: 2012-2023 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QObject>

#include "setupmanager.h"

class Ispdb;

class PersonalDataObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString incomingServer READ incomingServer NOTIFY incomingServerChanged)
    Q_PROPERTY(QString incomingProtocol READ incomingProtocol NOTIFY incomingProtocolChanged)
    Q_PROPERTY(QString outcomingServer READ outcomingServer NOTIFY outcomingServer)
public:
    explicit PersonalDataPage(QObject *parent = nullptr);
    ~PersonalDataObject();

    void setHideOptionInternetSearch(bool);

Q_SIGNALS:
    void manualWanted(bool);

public Q_SLOTS:
    void ispdbSearchFinished(bool ok);
    void slotTextChanged();
    void slotCreateAccountClicked();
    void slotRadioButtonClicked(const QString &protocol);
    void slotSearchType(const QString &);
    void automaticConfigureAccount();
    void configureSmtpAccount();
    void configureImapAccount();
    void configurePop3Account();

private:
    Ui::PersonalDataPage ui;
    Ispdb *mIspdb = nullptr;
    SetupManager *const mSetupManager;
};

