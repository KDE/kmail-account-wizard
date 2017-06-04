/*
    Copyright (c) 2016 Klarälvdalens Datakonsult AB

    This library is free software; you can redistribute it and/or modify it
    under the terms of the GNU Library General Public License as published by
    the Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    This library is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
    License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to the
    Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
    02110-1301, USA.
*/

#ifndef CRYPTOPAGE_H
#define CRYPTOPAGE_H

#include "page.h"
#include "ui_cryptopage.h"
#include "key.h"

#include <QPointer>

class Dialog;
class SetupManager;
class KeyGenerationJob;

namespace QGpgME {
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
    enum PublishingOptionPage {
        CheckingkWKSPage,
        WKSPage,
        PKSPage
    };

    enum Action {
        NoKey = 1,
        GenerateKey,
        ImportKey
    };

    void setPublishingEnabled(bool enabled);
    void importKey();
    Key::PublishingMethod currentPublishingMethod() const;

    Ui::CryptoPage ui;
    SetupManager *mSetupManager;
    QPointer<KeyGenerationJob> mKeyGenerationJob;
};

#endif // CRYPTOPAGE_H
