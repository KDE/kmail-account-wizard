/*
    SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "cryptopage.h"
#include "accountwizard_debug.h"
#include "dialog.h"
#include "identity.h"
#include "transport.h"

#include <Libkleo/Classify>
#include <Libkleo/DefaultKeyFilter>
#include <Libkleo/DefaultKeyGenerationJob>
#include <Libkleo/ProgressDialog>
#include <QGpgME/ImportJob>
#include <QGpgME/Job>
#include <QGpgME/KeyListJob>
#include <QGpgME/Protocol>
#include <QGpgME/WKSPublishJob>

#include <gpgme++/context.h>
#include <gpgme++/importresult.h>
#include <gpgme++/keygenerationresult.h>
#include <gpgme++/keylistresult.h>

#include <KMessageBox>
#include <KNewPasswordDialog>
#include <KNotifications/KNotification>

#include <KIdentityManagementCore/Identity>
#include <KIdentityManagementCore/IdentityManager>

#include <QEventLoopLocker>
#include <QFileDialog>

class KeyGenerationJob : public QObject
{
    Q_OBJECT

public:
    KeyGenerationJob(SetupManager *setupManager, const QString &passphrase, Key::PublishingMethod publishingMethod)
        : mSetupManager(setupManager)
        , mName(setupManager->name())
        , mEmail(setupManager->email())
        , mPassphrase(passphrase)
        , mTransportId(0)
        , mPublishingMethod(publishingMethod)
    {
        // Listen for when setup of Transport is finished so we can snatch
        // the transport ID
        connect(mSetupManager.data(), &SetupManager::setupFinished, this, &KeyGenerationJob::onObjectSetupFinished);

        qCDebug(ACCOUNTWIZARD_LOG) << "Starting key generation";
        auto job = new Kleo::DefaultKeyGenerationJob(this);
        job->setPassphrase(mPassphrase);
        connect(job, &Kleo::DefaultKeyGenerationJob::result, this, &KeyGenerationJob::keyGenerated);
        job->start(mEmail, mName);
    }

    ~KeyGenerationJob() override = default;

Q_SIGNALS:
    void result(const QString &fingerprint);

private Q_SLOTS:
    void onObjectSetupFinished(SetupObject *obj)
    {
        if (auto *transport = qobject_cast<Transport *>(obj)) {
            mTransportId = transport->transportId();
        }
    }

    void keyGenerated(const GpgME::KeyGenerationResult &result)
    {
        if (result.error()) {
            qCWarning(ACCOUNTWIZARD_LOG) << "Key generation finished with error:" << result.error().asString();
            KNotification::event(KNotification::Error,
                                 i18n("Account Wizard"),
                                 i18n("Error while generating new key pair for your account %1: %2", mEmail, QString::fromUtf8(result.error().asString())),
                                 QStringLiteral("akonadi"));
            deleteLater();
            return;
        }

        const QString fpr = QString::fromLatin1(result.fingerprint());
        qCDebug(ACCOUNTWIZARD_LOG) << "Finished generating key" << fpr;
        Q_EMIT this->result(fpr);

        auto listJob = QGpgME::openpgp()->keyListJob(false, true, true);
        connect(listJob, &QGpgME::KeyListJob::result, this, &KeyGenerationJob::keyRetrieved);
        listJob->start({fpr}, true);
    }

    void keyRetrieved(const GpgME::KeyListResult &result, const std::vector<GpgME::Key> &keys)
    {
        if (result.error() || keys.size() != 1) {
            qCWarning(ACCOUNTWIZARD_LOG) << "Key listing finished with error;" << result.error().asString();
            KNotification::event(KNotification::Error,
                                 i18n("Account Wizard"),
                                 i18n("Error while generating new key pair for your account %1: %2", mEmail, QString::fromUtf8(result.error().asString())),
                                 QStringLiteral("akonadi"));
            deleteLater();
            return;
        }

        qCDebug(ACCOUNTWIZARD_LOG) << "Post-generation key listing finished";

        const auto key = keys[0];

        // SetupManager no longer exists -> all is set up, so we need to modify
        // the Identity ourselves
        if (!mSetupManager) {
            qDebug(ACCOUNTWIZARD_LOG) << "SetupManager no longer exists!";
            updateIdentity(mEmail, key.primaryFingerprint());
            publishKeyIfNeeded(key);
            return;
        }

        {
            // SetupManager still lives, see if Identity has already been set up.
            // If not then pass it the new key and let it set up everything
            const auto objectsToSetup = mSetupManager->objectsToSetup();
            const auto identIt = std::find_if(objectsToSetup.cbegin(), objectsToSetup.cend(), [](SetupObject *obj) -> bool {
                return qobject_cast<Identity *>(obj);
            });
            const auto keyIt = std::find_if(objectsToSetup.cbegin(), objectsToSetup.cend(), [](SetupObject *obj) -> bool {
                return qobject_cast<Key *>(obj);
            });
            if (identIt != objectsToSetup.cend()) {
                qCDebug(ACCOUNTWIZARD_LOG) << "Identity not set up yet, less work for us";
                qobject_cast<Identity *>(*identIt)->setKey(GpgME::OpenPGP, key.primaryFingerprint());
                // The Key depends on Identity, so if Identity wasn't set up
                // yet, neither was the Key.
                if (mPublishingMethod != Key::NoPublishing && keyIt != objectsToSetup.cend()) {
                    auto keyObj = qobject_cast<Key *>(*keyIt);
                    keyObj->setKey(key);
                    keyObj->setPublishingMethod(mPublishingMethod);
                    keyObj->setMailBox(mEmail);
                }
                return;
            }
        }

        {
            // SetupManager still lives, but Identity has already been set up,
            // so update it.
            const auto setupObjects = mSetupManager->setupObjects();
            const auto it = std::find_if(setupObjects.cbegin(), setupObjects.cend(), [](SetupObject *obj) -> bool {
                return qobject_cast<Identity *>(obj);
            });
            if (it != setupObjects.cend()) {
                qCDebug(ACCOUNTWIZARD_LOG) << "Identity already set up, will modify existing Identity";
                updateIdentity(mEmail, key.primaryFingerprint());
                // Too late, we must publish the key ourselves now.
                publishKeyIfNeeded(key);
                return;
            }
        }

        // SetupManager still lives, but Identity is not pending nor finished,
        // which means it was not even prepared yet in SetupManager. This will
        // also handle publishing for us if needed.
        qCDebug(ACCOUNTWIZARD_LOG) << "Identity not ready yet, passing the key to SetupManager";
        mSetupManager->setKey(key);
        mSetupManager->setKeyPublishingMethod(mPublishingMethod);
        deleteLater();
    }

    void updateIdentity(const QString &email, const QByteArray &fingerprint)
    {
        auto manager = KIdentityManagementCore::IdentityManager::self();
        for (auto it = manager->modifyBegin(), end = manager->modifyEnd(); it != end; ++it) {
            if (it->primaryEmailAddress() == email) {
                qCDebug(ACCOUNTWIZARD_LOG) << "Found matching identity for" << email << ":" << it->uoid();
                it->setPGPSigningKey(fingerprint);
                it->setPGPEncryptionKey(fingerprint);
                manager->commit();
                return;
            }
        }
        manager->rollback();

        qCWarning(ACCOUNTWIZARD_LOG) << "What? Could not find a matching identity for" << email << "!";
    }

    void publishKeyIfNeeded(const GpgME::Key &key)
    {
        if (mPublishingMethod == Key::NoPublishing) {
            qCDebug(ACCOUNTWIZARD_LOG) << "Key publishing not requested, we are done";
            deleteLater();
            return;
        }

        auto keyObj = new Key(mSetupManager); // mSetupManager can be null, but that's OK
        keyObj->setKey(key);
        keyObj->setPublishingMethod(mPublishingMethod);
        keyObj->setMailBox(mEmail);
        keyObj->setTransportId(mTransportId);
        connect(keyObj, &Key::error, this, [this](const QString &msg) {
            KNotification::event(KNotification::Error, i18n("Account Wizard"), msg, QStringLiteral("akonadi"));
            deleteLater();
        });
        connect(keyObj, &Key::finished, this, &KeyGenerationJob::deleteLater);
        keyObj->create();
    }

private:
    // This job may outlive the application, make sure QApplication won't
    // quit before we are done
    QEventLoopLocker mLocker;
    QPointer<SetupManager> mSetupManager;
    QString mName;
    QString mEmail;
    QString mPassphrase;
    int mTransportId;
    Key::PublishingMethod mPublishingMethod;
};

class KeyImportJob : public QGpgME::Job
{
    Q_OBJECT
public:
    KeyImportJob(const QString &file, Kleo::KeySelectionCombo *parent)
        : QGpgME::Job(parent)
        , mFile(file)
        , mJob(nullptr)
    {
    }

    ~KeyImportJob() override = default;

    void slotCancel() override
    {
        if (mJob) {
            mJob->slotCancel();
        }
    }

    void start()
    {
        QGpgME::ImportJob *job = nullptr;
        switch (Kleo::findProtocol(mFile)) {
        case GpgME::OpenPGP:
            job = QGpgME::openpgp()->importJob();
            break;
        case GpgME::CMS:
            job = QGpgME::smime()->importJob();
            break;
        default:
            job = nullptr;
            break;
        }

        if (!job) {
            KMessageBox::error(qobject_cast<QWidget *>(parent()), i18n("Could not detect valid key type"), i18n("Import error"));
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

        connect(job, &QGpgME::ImportJob::result, this, &KeyImportJob::keyImported);
        job->start(keyFile.readAll());
        mJob = job;
    }

    void keyImported(const GpgME::ImportResult &result)
    {
        mJob = nullptr;
        if (result.error()) {
            KMessageBox::error(qobject_cast<QWidget *>(parent()),
                               i18n("Failed to import key: %1", QString::fromUtf8(result.error().asString())),
                               i18n("Import error"));
            Q_EMIT done();
            return;
        }

        const auto imports = result.imports();
        if (imports.size() == 0) {
            KMessageBox::error(qobject_cast<QWidget *>(parent()), i18n("Failed to import key."), i18n("Import error"));
            Q_EMIT done();
            return;
        }

        auto combo = qobject_cast<Kleo::KeySelectionCombo *>(parent());
        combo->setDefaultKey(QLatin1String(result.import(0).fingerprint()));
        connect(combo, &Kleo::KeySelectionCombo::keyListingFinished, this, &KeyImportJob::done);
        combo->refreshKeys();
    }

private:
    QString mFile;
    QGpgME::Job *mJob = nullptr;
};

CryptoPage::CryptoPage(Dialog *parent)
    : Page(parent)
    , mSetupManager(parent->setupManager())
{
    ui.setupUi(this);

    std::shared_ptr<Kleo::DefaultKeyFilter> filter(new Kleo::DefaultKeyFilter);
    filter->setCanSign(Kleo::DefaultKeyFilter::Set);
    filter->setCanEncrypt(Kleo::DefaultKeyFilter::Set);
    filter->setHasSecret(Kleo::DefaultKeyFilter::Set);
    ui.keyCombo->setKeyFilter(filter);
    ui.keyCombo->prependCustomItem(QIcon(), i18n("No key"), NoKey);
    ui.keyCombo->appendCustomItem(QIcon::fromTheme(QStringLiteral("document-new")), i18n("Generate a new key pair"), GenerateKey);
    ui.keyCombo->appendCustomItem(QIcon::fromTheme(QStringLiteral("document-import")), i18n("Import a key"), ImportKey);

    connect(ui.keyCombo, &Kleo::KeySelectionCombo::customItemSelected, this, &CryptoPage::customItemSelected);
    connect(ui.keyCombo, &Kleo::KeySelectionCombo::currentKeyChanged, this, &CryptoPage::keySelected);
}

void CryptoPage::enterPageNext()
{
    ui.keyCombo->setIdFilter(mSetupManager->email());
    if (ui.keyCombo->count() == 3) {
        // No key + Generate key + Import key options, no actual keys, so
        // pre-select the "Generate key" option
        const int idx = ui.keyCombo->findData(GenerateKey, Qt::UserRole);
        ui.keyCombo->setCurrentIndex(idx);
    } else {
        // We have at least one key, pre-select it
        // Index 0 is "No key" option, so index 1 will be a key
        ui.keyCombo->setCurrentIndex(1);
    }

    ui.stackedWidget->setCurrentIndex(CheckingkWKSPage);
    auto job = QGpgME::openpgp()->wksPublishJob();
    connect(job, &QGpgME::WKSPublishJob::result, this, [this](const GpgME::Error &error) {
        if (error) {
            ui.stackedWidget->setCurrentIndex(PKSPage);
        } else {
            ui.stackedWidget->setCurrentIndex(WKSPage);
        }
    });
    job->startCheck(mSetupManager->email());
}

void CryptoPage::leavePageNext()
{
    const auto key = ui.keyCombo->currentKey();
    if (!key.isNull()) {
        mSetupManager->setKey(key);
        mSetupManager->setKeyPublishingMethod(currentPublishingMethod());
    } else if (ui.keyCombo->currentData(Qt::UserRole).toInt() == GenerateKey) {
        if (!mKeyGenerationJob) {
            mKeyGenerationJob = new KeyGenerationJob(mSetupManager, ui.passwordWidget->password(), currentPublishingMethod());
            ui.keyCombo->setEnabled(false); // disable until key is generated
            ui.passwordWidget->setEnabled(false);
            connect(mKeyGenerationJob.data(), &KeyGenerationJob::result, this, [this](const QString &fpr) {
                ui.keyCombo->setEnabled(true);
                ui.passwordWidget->setEnabled(true);
                ui.keyCombo->setDefaultKey(fpr);
                ui.keyCombo->refreshKeys();
            });
        }
    }
    mSetupManager->setPgpAutoEncrypt(ui.enableCryptoCheckBox->isChecked());
    mSetupManager->setPgpAutoSign(ui.enableCryptoCheckBox->isChecked());
}

Key::PublishingMethod CryptoPage::currentPublishingMethod() const
{
    if (ui.stackedWidget->currentIndex() == PKSPage && ui.pksCheckBox->isChecked()) {
        return Key::PKS;
    } else if (ui.stackedWidget->currentIndex() == WKSPage && ui.wksCheckBox->isChecked()) {
        return Key::WKS;
    } else {
        return Key::NoPublishing;
    }
}

void CryptoPage::customItemSelected(const QVariant &data)
{
    switch (data.toInt()) {
    case NoKey:
        setValid(true);
        setPublishingEnabled(false);
        ui.passwordWidget->setVisible(false);
        return;
    case GenerateKey:
        setValid(true);
        setPublishingEnabled(true);
        ui.passwordWidget->setVisible(true);
        break;
    case ImportKey:
        setValid(false);
        setPublishingEnabled(true);
        ui.passwordWidget->setVisible(false);
        importKey();
        break;
    }
}

void CryptoPage::keySelected(const GpgME::Key &key)
{
    ui.passwordWidget->setVisible(false);

    // We don't support publishing for S/MIME
    setPublishingEnabled(key.protocol() == GpgME::OpenPGP);
    setValid(!key.isNull());
}

void CryptoPage::setPublishingEnabled(bool enabled)
{
    ui.wksCheckBox->setEnabled(enabled);
    ui.wksCheckBox->setChecked(enabled);
    ui.pksCheckBox->setEnabled(enabled);
    ui.pksCheckBox->setChecked(enabled);
}

void CryptoPage::importKey()
{
    const QString certificateFilter = i18n("Certificates") + QLatin1String(" (*.asc *.cer *.cert *.crt *.der *.pem *.gpg *.p7c *.p12 *.pfx *.pgp)");
    const QString anyFilesFilter = i18n("Any files") + QLatin1String(" (*)");

    const QString file =
        QFileDialog::getOpenFileName(this, i18n("Select Certificate File"), QString(), certificateFilter + QLatin1String(";;") + anyFilesFilter);
    if (file.isEmpty()) {
        return;
    }

    auto job = new KeyImportJob(file, ui.keyCombo);
    new Kleo::ProgressDialog(job, i18n("Importing key..."), ui.keyCombo->parentWidget());
    ui.keyCombo->setEnabled(false);
    QObject::connect(job, &KeyImportJob::done, ui.keyCombo, [this]() {
        ui.keyCombo->setEnabled(true);
    });
    job->start();
}

#include "cryptopage.moc"

#include "moc_cryptopage.cpp"
