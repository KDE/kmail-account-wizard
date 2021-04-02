/*
    SPDX-FileCopyrightText: 2009 Volker Krause <vkrause@kde.org>
    SPDX-FileCopyrightText: 2010 Tom Albers <toma@kde.org>
    SPDX-FileCopyrightText: 2012-2021 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "personaldatapage.h"

#include "global.h"
#include "ispdb/ispdb.h"
#include "resource.h"
#include "transport.h"
#include <config-enterprise.h>

#include <KAuthorized>
#include <KEmailAddress>
#include <PimCommon/EmailValidator>

#include <MailTransport/Transport>

#include "accountwizard_debug.h"

QString accountName(Ispdb *ispdb, QString username)
{
    const int pos(username.indexOf(QLatin1Char('@')));
    username = username.left(pos);
    return ispdb->name(Ispdb::Long) + QStringLiteral(" (%1)").arg(username);
}

PersonalDataPage::PersonalDataPage(Dialog *parent)
    : Page(parent)
    , mIspdb(nullptr)
    , mSetupManager(parent->setupManager())
{
    QWidget *pageParent = this;

    ui.setupUi(pageParent);
    ui.mProgress->stop();
    ui.passwordEdit->setRevealPasswordAvailable(KAuthorized::authorize(QStringLiteral("lineedit_reveal_password")));

    auto *emailValidator = new PimCommon::EmailValidator(this);
    ui.emailEdit->setValidator(emailValidator);

    // KEmailSettings defaults
    ui.nameEdit->setText(mSetupManager->name());
    ui.emailEdit->setText(mSetupManager->email());
    slotTextChanged();
    connect(ui.emailEdit, &QLineEdit::textChanged, this, &PersonalDataPage::slotTextChanged);
    connect(ui.nameEdit, &QLineEdit::textChanged, this, &PersonalDataPage::slotTextChanged);
    connect(ui.createAccountPb, &QPushButton::clicked, this, &PersonalDataPage::slotCreateAccountClicked);
    connect(ui.buttonGroup, &QButtonGroup::buttonClicked, this, &PersonalDataPage::slotRadioButtonClicked);
#ifdef KDEPIM_ENTERPRISE_BUILD
    ui.checkOnlineGroupBox->setChecked(false);
#endif
}

void PersonalDataPage::setHideOptionInternetSearch(bool hide)
{
    ui.checkOnlineGroupBox->setChecked(!hide);
    ui.checkOnlineGroupBox->setVisible(!hide);
}

void PersonalDataPage::slotRadioButtonClicked(QAbstractButton *button)
{
    QString smptHostname;
    if (!mIspdb->smtpServers().isEmpty()) {
        const Server s = mIspdb->smtpServers().at(0);
        smptHostname = s.hostname;
    }
    ui.outgoingLabel->setText(i18n("SMTP, %1", smptHostname));
    if (button == ui.imapAccount) {
        const Server simap = mIspdb->imapServers().at(0); // should be ok.
        ui.incommingLabel->setText(i18n("IMAP, %1", simap.hostname));
        ui.usernameLabel->setText(simap.username);
    } else if (button == ui.pop3Account) {
        const Server spop3 = mIspdb->pop3Servers().at(0); // should be ok.
        ui.incommingLabel->setText(i18n("POP3, %1", spop3.hostname));
        ui.usernameLabel->setText(spop3.username);
    }
}

void PersonalDataPage::slotCreateAccountClicked()
{
    configureSmtpAccount();
    if (ui.imapAccount->isChecked()) {
        configureImapAccount();
    } else {
        configurePop3Account();
    }
    Q_EMIT leavePageNextOk(); // go to the next page
    mSetupManager->execute();
}

void PersonalDataPage::slotTextChanged()
{
    // Ignore the password field, as that can be empty when auth is based on ip-address.
    setValid(!ui.emailEdit->text().isEmpty() && !ui.nameEdit->text().isEmpty() && KEmailAddress::isValidSimpleAddress(ui.emailEdit->text()));
}

void PersonalDataPage::leavePageNext()
{
    ui.stackedPage->setCurrentIndex(0);
    ui.imapAccount->setChecked(true);
    mSetupManager->setPersonalDataAvailable(true);
    mSetupManager->setName(ui.nameEdit->text());
    mSetupManager->setPassword(ui.passwordEdit->password());
    mSetupManager->setEmail(ui.emailEdit->text().trimmed());

    if (ui.checkOnlineGroupBox->isChecked()) {
        // since the user can go back and forth, explicitly disable the man page
        Q_EMIT manualWanted(false);
        setCursor(Qt::BusyCursor);
        ui.mProgress->start();
        qCDebug(ACCOUNTWIZARD_LOG) << "Searching on internet";
        delete mIspdb;
        mIspdb = new Ispdb(this);
        connect(mIspdb, &Ispdb::searchType, this, &PersonalDataPage::slotSearchType);
        mIspdb->setEmail(ui.emailEdit->text());
        mIspdb->start();

        connect(mIspdb, &Ispdb::finished, this, &PersonalDataPage::ispdbSearchFinished);
    } else {
        Q_EMIT manualWanted(true); // enable the manual page
        Q_EMIT leavePageNextOk(); // go to the next page
    }
}

void PersonalDataPage::ispdbSearchFinished(bool ok)
{
    qCDebug(ACCOUNTWIZARD_LOG) << ok;

    unsetCursor();
    ui.mProgress->stop();
    if (ok) {
        if (!mIspdb->imapServers().isEmpty() && !mIspdb->pop3Servers().isEmpty()) {
            ui.stackedPage->setCurrentIndex(1);
            slotRadioButtonClicked(ui.imapAccount);
        } else {
            automaticConfigureAccount();
        }
    } else {
        Q_EMIT manualWanted(true); // enable the manual page
        Q_EMIT leavePageNextOk();
    }
}

void PersonalDataPage::slotSearchType(const QString &type)
{
    ui.mProgress->setActiveLabel(type);
}

void PersonalDataPage::leavePageNextRequested()
{
    // override base class with doing nothing...
}
