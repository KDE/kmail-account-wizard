/*
    SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "key.h"
#include "page.h"
#include "ui_cryptopage.h"

#include <QPointer>

class Dialog;
class SetupManager;
class KeyGenerationJob;

namespace QGpgME
{
class Job;
}

class CryptoPage : public Page
{
    Q_OBJECT

public:
    explicit CryptoPage(Dialog *parent);

    void enterPageNext() override;
    void leavePageNext() override;

private Q_SLOTS:
    void customItemSelected(const QVariant &data);
    void keySelected(const GpgME::Key &key);

private:
    enum PublishingOptionPage { CheckingkWKSPage, WKSPage, PKSPage };

    enum Action { NoKey = 1, GenerateKey, ImportKey };

    void setPublishingEnabled(bool enabled);
    void importKey();
    Key::PublishingMethod currentPublishingMethod() const;

    Ui::CryptoPage ui;
    SetupManager *mSetupManager = nullptr;
    QPointer<KeyGenerationJob> mKeyGenerationJob;
};

