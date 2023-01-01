/*
    SPDX-FileCopyrightText: 2009 Volker Krause <vkrause@kde.org>
    SPDX-FileCopyrightText: 2010 Tom Albers <toma@kde.org>
    SPDX-FileCopyrightText: 2012-2023 Laurent Montel <montel@kde.org>

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
#if KDEPIM_ENTERPRISE_BUILD
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

void PersonalDataPage::configureSmtpAccount()
{
    if (!mIspdb->smtpServers().isEmpty()) {
        const Server s = mIspdb->smtpServers().at(0); // should be ok.
        qCDebug(ACCOUNTWIZARD_LOG) << "Configuring transport for" << s.hostname;

        QObject *object = mSetupManager->createTransport(QStringLiteral("smtp"));
        auto *t = qobject_cast<Transport *>(object);
        t->setName(accountName(mIspdb, s.username));
        t->setHost(s.hostname);
        t->setPort(s.port);
        t->setUsername(s.username);
        t->setPassword(ui.passwordEdit->password());
        switch (s.authentication) {
        case Ispdb::Plain:
            t->setAuthenticationType(QStringLiteral("plain"));
            break;
        case Ispdb::CramMD5:
            t->setAuthenticationType(QStringLiteral("cram-md5"));
            break;
        case Ispdb::NTLM:
            t->setAuthenticationType(QStringLiteral("ntlm"));
            break;
        case Ispdb::GSSAPI:
            t->setAuthenticationType(QStringLiteral("gssapi"));
            break;
        case Ispdb::OAuth2:
            t->setAuthenticationType(QStringLiteral("oauth2"));
            break;
        case Ispdb::ClientIP:
            break;
        case Ispdb::NoAuth:
            break;
        default:
            break;
        }
        switch (s.socketType) {
        case Ispdb::None:
            t->setEncryption(QStringLiteral("none"));
            break;
        case Ispdb::SSL:
            t->setEncryption(QStringLiteral("ssl"));
            break;
        case Ispdb::StartTLS:
            t->setEncryption(QStringLiteral("tls"));
            break;
        default:
            break;
        }
    } else {
        qCDebug(ACCOUNTWIZARD_LOG) << "No transport to be created....";
    }
}

void PersonalDataPage::configureImapAccount()
{
    if (!mIspdb->imapServers().isEmpty()) {
        Server s = mIspdb->imapServers().at(0); // should be ok.
        qCDebug(ACCOUNTWIZARD_LOG) << "Configuring imap for" << s.hostname;

        QObject *object = mSetupManager->createResource(QStringLiteral("akonadi_imap_resource"));
        auto *t = qobject_cast<Resource *>(object);
        t->setName(accountName(mIspdb, s.username));
        t->setOption(QStringLiteral("ImapServer"), s.hostname);
        t->setOption(QStringLiteral("ImapPort"), s.port);
        t->setOption(QStringLiteral("UserName"), s.username);
        t->setOption(QStringLiteral("Password"), ui.passwordEdit->password());
        switch (s.authentication) {
        case Ispdb::Plain:
            t->setOption(QStringLiteral("Authentication"), MailTransport::Transport::EnumAuthenticationType::CLEAR);
            break;
        case Ispdb::CramMD5:
            t->setOption(QStringLiteral("Authentication"), MailTransport::Transport::EnumAuthenticationType::CRAM_MD5);
            break;
        case Ispdb::NTLM:
            t->setOption(QStringLiteral("Authentication"), MailTransport::Transport::EnumAuthenticationType::NTLM);
            break;
        case Ispdb::GSSAPI:
            t->setOption(QStringLiteral("Authentication"), MailTransport::Transport::EnumAuthenticationType::GSSAPI);
            break;
        case Ispdb::ClientIP:
            break;
        case Ispdb::NoAuth:
            break;
        case Ispdb::OAuth2:
            t->setOption(QStringLiteral("Authentication"), MailTransport::Transport::EnumAuthenticationType::XOAUTH2);
            break;
        default:
            break;
        }
        switch (s.socketType) {
        case Ispdb::None:
            t->setOption(QStringLiteral("Safety"), QStringLiteral("None"));
            break;
        case Ispdb::SSL:
            t->setOption(QStringLiteral("Safety"), QStringLiteral("SSL"));
            break;
        case Ispdb::StartTLS:
            t->setOption(QStringLiteral("Safety"), QStringLiteral("STARTTLS"));
            break;
        default:
            break;
        }
    }
}

void PersonalDataPage::configurePop3Account()
{
    if (!mIspdb->pop3Servers().isEmpty()) {
        Server s = mIspdb->pop3Servers().at(0); // should be ok.
        qCDebug(ACCOUNTWIZARD_LOG) << "No Imap to be created, configuring pop3 for" << s.hostname;

        QObject *object = mSetupManager->createResource(QStringLiteral("akonadi_pop3_resource"));
        auto *t = qobject_cast<Resource *>(object);
        t->setName(accountName(mIspdb, s.username));
        t->setOption(QStringLiteral("Host"), s.hostname);
        t->setOption(QStringLiteral("Port"), s.port);
        t->setOption(QStringLiteral("Login"), s.username);
        t->setOption(QStringLiteral("Password"), ui.passwordEdit->password());
        switch (s.authentication) {
        case Ispdb::Plain:
            t->setOption(QStringLiteral("AuthenticationMethod"), MailTransport::Transport::EnumAuthenticationType::PLAIN);
            break;
        case Ispdb::CramMD5:
            t->setOption(QStringLiteral("AuthenticationMethod"), MailTransport::Transport::EnumAuthenticationType::CRAM_MD5);
            break;
        case Ispdb::NTLM:
            t->setOption(QStringLiteral("AuthenticationMethod"), MailTransport::Transport::EnumAuthenticationType::NTLM);
            break;
        case Ispdb::GSSAPI:
            t->setOption(QStringLiteral("AuthenticationMethod"), MailTransport::Transport::EnumAuthenticationType::GSSAPI);
            break;
        case Ispdb::ClientIP:
        case Ispdb::NoAuth:
        default:
            t->setOption(QStringLiteral("AuthenticationMethod"), MailTransport::Transport::EnumAuthenticationType::CLEAR);
            break;
        }
        switch (s.socketType) {
        case Ispdb::SSL:
            t->setOption(QStringLiteral("UseSSL"), 1);
            break;
        case Ispdb::StartTLS:
            t->setOption(QStringLiteral("UseTLS"), 1);
            break;
        case Ispdb::None:
        default:
            t->setOption(QStringLiteral("UseTLS"), 1);
            break;
        }
    }
}

void PersonalDataPage::automaticConfigureAccount()
{
    configureSmtpAccount();
    configureImapAccount();
    configurePop3Account();
    Q_EMIT leavePageNextOk(); // go to the next page
    mSetupManager->execute();
}

void PersonalDataPage::leavePageNextRequested()
{
    // override base class with doing nothing...
}
