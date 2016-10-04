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

#include "cryptopage.h"
#include "dialog.h"
#include "identity.h"
#include "accountwizard_debug.h"

#include <Libkleo/DefaultKeyFilter>
#include <Libkleo/Job>
#include <Libkleo/ImportJob>
#include <Libkleo/CryptoBackendFactory>
#include <Libkleo/DefaultKeyGenerationJob>
#include <Libkleo/ProgressDialog>
#include <Libkleo/Classify>

#include <gpgme++/context.h>
#include <gpgme++/keygenerationresult.h>
#include <gpgme++/importresult.h>

#include <KMessageBox>
#include <KNotifications/KNotification>
#include <KNewPasswordDialog>

#include <KIdentityManagement/IdentityManager>
#include <KIdentityManagement/Identity>

#include <QFileDialog>
#include <QEventLoopLocker>
#include <QPointer>

class DeleteLaterRAII
{
public:
    DeleteLaterRAII(QObject *obj) : mObj(obj)
    {}
    ~DeleteLaterRAII()
    {
        if (mObj) mObj->deleteLater();
    }
private:
    Q_DISABLE_COPY(DeleteLaterRAII)
    QPointer<QObject> mObj;
};

class KeyGenerationJob : public QObject
{
    Q_OBJECT

public:
    KeyGenerationJob(SetupManager *setupManager, const QString &passphrase)
        : mSetupManager(setupManager)
        , mEmail(setupManager->email())
    {
        qCDebug(ACCOUNTWIZARD_LOG) << "Starting key generation";
        auto job = new Kleo::DefaultKeyGenerationJob(this);
        job->setPassphrase(passphrase);
        connect(job, &Kleo::DefaultKeyGenerationJob::result,
                this, &KeyGenerationJob::keyGenerated);
        job->start(setupManager->name(), setupManager->email());
    }

    ~KeyGenerationJob()
    {
    }

private Q_SLOTS:
    void keyGenerated(const GpgME::KeyGenerationResult &result)
    {
        DeleteLaterRAII deleter(this);

        if (result.error()) {
            qCWarning(ACCOUNTWIZARD_LOG) << "Key generation finished with error:" << result.error().asString();
            KNotification::event(KNotification::Error,
                                 i18n("Account Wizard"),
                                 i18n("Error while generating new key pair for your account %1: %2",
                                      mEmail, QString::fromUtf8(result.error().asString())),
                                 QStringLiteral("akonadi"));
            return;
        }

        qCDebug(ACCOUNTWIZARD_LOG) << "Finished generating key" << result.fingerprint();

        // SetupManager no longer exists -> all is set up, so we need to modify
        // the Identity ourselves
        if (!mSetupManager) {
            qDebug(ACCOUNTWIZARD_LOG) << "SetupManager no longer exists!";
            updateIdentity(mEmail, result.fingerprint());
            return;
        }

        // SetupManager still lives, see if Identity has already been set up.
        // If not then pass it the new key and let it to set up everything
        const auto objectsToSetup = mSetupManager->objectsToSetup();
        for (auto object : objectsToSetup) {
            if (Identity *identity = qobject_cast<Identity*>(object)) {
                qCDebug(ACCOUNTWIZARD_LOG) << "Identity not set up yet, less work for us";
                identity->setKey(GpgME::OpenPGP, result.fingerprint());
                return;
            }
        }

        // SetupManager still lives, but Identity either was not even created yet
        // or has already been set up
        const auto setupObjects = mSetupManager->setupObjects();
        for (auto object : setupObjects) {
            if (qobject_cast<Identity*>(object)) {
                qCDebug(ACCOUNTWIZARD_LOG) << "Identity already set up, will modify existing Identity";
                updateIdentity(mEmail, result.fingerprint());
                return;
            }
        }

        // SetupManager still lives, but Identity is not pending nor finished,
        // which means it was not even prepared yet in SetupManager.
        qCDebug(ACCOUNTWIZARD_LOG) << "Identity not ready yet, passing the key to SetupManager";
        mSetupManager->setKey(GpgME::OpenPGP, result.fingerprint());
    }

    void updateIdentity(const QString &email, const QByteArray &fingerprint)
    {
        auto manager = KIdentityManagement::IdentityManager::self();
        for (auto it = manager->modifyBegin(), end = manager->modifyEnd(); it != end; ++it) {
            if (it->primaryEmailAddress() == email) {
                qCDebug(ACCOUNTWIZARD_LOG) << "Found matching identity for" << email <<":" << it->uoid();
                it->setPGPSigningKey(fingerprint);
                it->setPGPEncryptionKey(fingerprint);
                manager->commit();
                return;
            }
        }

        qCWarning(ACCOUNTWIZARD_LOG) << "What? Could not find a matching identity for" << email << "!";
    }

private:
    // This job may outlive the application, make sure QApplication won't
    // quit before we are done
    QEventLoopLocker mLocker;
    QPointer<SetupManager> mSetupManager;
    QString mEmail;
};

class KeyImportJob : public Kleo::Job
{
    Q_OBJECT
public:
    KeyImportJob(const QString &file, Kleo::KeySelectionCombo *parent)
        : Kleo::Job(parent)
        , mFile(file)
        , mJob(Q_NULLPTR)
    {
    }

    ~KeyImportJob()
    {
    }

    void slotCancel() Q_DECL_OVERRIDE {
        if (mJob)
        {
            mJob->slotCancel();
        }
    }

    void start()
    {
        Kleo::ImportJob *job = Q_NULLPTR;
        switch (Kleo::findProtocol(mFile)) {
        case GpgME::OpenPGP:
            job = Kleo::CryptoBackendFactory::instance()->openpgp()->importJob();
            break;
        case GpgME::CMS:
            job = Kleo::CryptoBackendFactory::instance()->smime()->importJob();
            break;
        default:
            job = Q_NULLPTR;
            break;
        }

        if (!job) {
            KMessageBox::error(qobject_cast<QWidget *>(parent()),
                               i18n("Could not detect valid key type"),
                               i18n("Import error"));
            Q_EMIT done();
            return;
        }

        QFile keyFile(mFile);
        if (!keyFile.open(QIODevice::ReadOnly)) {
            KMessageBox::error(qobject_cast<QWidget *>(parent()),
                               i18n("Cannot read data from the certificate file: %1", keyFile.errorString()),
                               i18n("Import error"));
            Q_EMIT done();
            return;
        }

        connect(job, &Kleo::ImportJob::result,
                this, &KeyImportJob::keyImported);
        job->start(keyFile.readAll());
        mJob = job;
    }

    void keyImported(const GpgME::ImportResult &result)
    {
        mJob = Q_NULLPTR;
        if (result.error()) {
            KMessageBox::error(qobject_cast<QWidget *>(parent()),
                               i18n("Failed to import key: %1", QString::fromUtf8(result.error().asString())),
                               i18n("Import error"));
            Q_EMIT done();
            return;
        }

        const auto imports = result.imports();
        if (imports.size() == 0) {
            KMessageBox::error(qobject_cast<QWidget *>(parent()),
                               i18n("Failed to import key."),
                               i18n("Import error"));
            Q_EMIT done();
            return;
        }

        auto combo = qobject_cast<Kleo::KeySelectionCombo *>(parent());
        combo->setDefaultKey(QLatin1String(result.import(0).fingerprint()));
        connect(combo, &Kleo::KeySelectionCombo::keyListingFinished,
                this, &KeyImportJob::done);
        combo->refreshKeys();
    }

private:
    QString mFile;
    Kleo::Job *mJob;
};

CryptoPage::CryptoPage(Dialog *parent)
    : Page(parent)
    , mSetupManager(parent->setupManager())
{
    ui.setupUi(this);
    // TODO: Enable once we implement key publishing
    ui.publishCheckbox->setChecked(false);
    ui.publishCheckbox->setEnabled(false);

    boost::shared_ptr<Kleo::DefaultKeyFilter> filter(new Kleo::DefaultKeyFilter);
    filter->setCanSign(Kleo::DefaultKeyFilter::Set);
    filter->setCanEncrypt(Kleo::DefaultKeyFilter::Set);
    filter->setHasSecret(Kleo::DefaultKeyFilter::Set);
    ui.keyCombo->setKeyFilter(filter);
    ui.keyCombo->prependCustomItem(QIcon(), i18n("No key"), NoKey);
    ui.keyCombo->appendCustomItem(QIcon::fromTheme(QStringLiteral("document-new")),
                                  i18n("Generate a new key pair"), GenerateKey);
    ui.keyCombo->appendCustomItem(QIcon::fromTheme(QStringLiteral("document-import")),
                                  i18n("Import a key"), ImportKey);

    connect(ui.keyCombo, &Kleo::KeySelectionCombo::customItemSelected,
            this, &CryptoPage::customItemSelected);
    connect(ui.keyCombo, &Kleo::KeySelectionCombo::currentKeyChanged,
    this, [this](const GpgME::Key & key) {
        setValid(!key.isNull());
    });
}

void CryptoPage::enterPageNext()
{
    ui.keyCombo->setIdFilter(mSetupManager->email());
}

void CryptoPage::leavePageNext()
{
    const auto key = ui.keyCombo->currentKey();
    mSetupManager->setKey(key.protocol(), key.primaryFingerprint());
}

void CryptoPage::customItemSelected(const QVariant &data)
{
    switch (data.toInt()) {
    case NoKey:
        setValid(true);
        return;
    case GenerateKey:
        setValid(false);
        generateKeyPair();
        break;
    case ImportKey:
        setValid(false);
        importKey();
        break;
    }
}

void CryptoPage::generateKeyPair()
{
    QScopedPointer<KNewPasswordDialog> dlg(new KNewPasswordDialog);
    dlg->setAllowEmptyPasswords(true);
    if (dlg->exec()) {
        new KeyGenerationJob(mSetupManager, dlg->password());
        nextPage();
    }
}

void CryptoPage::importKey()
{
    const QString certificateFilter = i18n("Certificates") + QLatin1String(" (*.asc *.cer *.cert *.crt *.der *.pem *.gpg *.p7c *.p12 *.pfx *.pgp)");
    const QString anyFilesFilter = i18n("Any files") + QLatin1String(" (*)");

    const QString file = QFileDialog::getOpenFileName(this, i18n("Select Certificate File"),
                         QString(),
                         certificateFilter + QLatin1String(";;") + anyFilesFilter);
    if (file.isEmpty()) {
        return;
    }

    auto job = new KeyImportJob(file, ui.keyCombo);
    new Kleo::ProgressDialog(job, i18n("Importing key..."), ui.keyCombo->parentWidget());
    ui.keyCombo->setEnabled(false);
    QObject::connect(job, &KeyImportJob::done,
                     ui.keyCombo, [this]() {
                        ui.keyCombo->setEnabled(true);
                     });
    job->start();
}

#include "cryptopage.moc"
