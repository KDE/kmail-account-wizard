/*
    SPDX-FileCopyrightText: 2009 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "setupmanager.h"
#include "configfile.h"
#include "identity.h"
#include "ispdbhelper.h"
#include "ldap.h"
#include "resource.h"
#include "setupautoconfigkolabfreebusy.h"
#include "setupautoconfigkolabldap.h"
#include "setupautoconfigkolabmail.h"
#include "transport.h"

#include <KEMailSettings>
#include <kwallet.h>

#include <QLocale>

SetupManager &SetupManager::instance()
{
    static SetupManager _setupManager;
    return _setupManager;
}

SetupManager::SetupManager(QObject *parent)
    : QObject(parent)
    , m_keyPublishingMethod(Key::NoPublishing)
{
    KEMailSettings e;
    m_name = e.getSetting(KEMailSettings::RealName);
    m_email = e.getSetting(KEMailSettings::EmailAddress);
}

SetupManager::~SetupManager()
{
    delete m_wallet;
}

Resource *SetupManager::createResource(const QString &type)
{
    return new Resource(type, this);
}

Transport *SetupManager::createTransport(const QString &type)
{
    return new Transport(type, this);
}

ConfigFile *SetupManager::createConfigFile(const QString &fileName)
{
    return new ConfigFile(fileName, this);
}

Ldap *SetupManager::createLdap()
{
    return new Ldap(this);
}

Identity *SetupManager::createIdentity()
{
    auto identity = new Identity(this);
    identity->setEmail(m_email);
    identity->setRealName(m_name);
    identity->setPgpAutoSign(m_pgpAutoSign);
    identity->setPgpAutoEncrypt(m_pgpAutoEncrypt);
    identity->setKey(m_key.protocol(), m_key.primaryFingerprint());
    return identity;
}

Key *SetupManager::createKey()
{
    Key *key = new Key(this);
    key->setKey(m_key);
    key->setMailBox(m_email);
    key->setPublishingMethod(m_keyPublishingMethod);
    return key;
}

QVector<SetupObject *> SetupManager::objectsToSetup() const
{
    return m_objectToSetup;
}

QVector<SetupObject *> SetupManager::setupObjects() const
{
    return m_setupObjects;
}

static bool dependencyCompare(SetupObject *left, SetupObject *right)
{
    if (!left->dependsOn() && right->dependsOn()) {
        return true;
    }
    return false;
}

void SetupManager::execute()
{
    qWarning() << " void SetupManager::execute() not implement yet";
    // if (m_keyPublishingMethod != Key::NoPublishing) {
    //    auto key = qobject_cast<Key *>(createKey());
    //    auto it = std::find_if(m_setupObjects.cbegin(), m_setupObjects.cend(), [](SetupObject *obj) -> bool {
    //        return qobject_cast<Transport *>(obj);
    //    });
    //    if (it != m_setupObjects.cend()) {
    //        key->setDependsOn(*it);
    //    }
    //}
    //
    //
    //// ### FIXME this is a bad over-simplification and would need a real topological sort
    //// but for current usage it is good enough
    // std::stable_sort(m_objectToSetup.begin(), m_objectToSetup.end(), dependencyCompare);
    // setupNext();
}

void SetupManager::setupSucceeded(const QString &msg)
{
    // Q_ASSERT(m_page);
    //// m_page->addMessage(SetupPage::Success, msg);
    // if (m_currentSetupObject) {
    //    Q_EMIT setupFinished(m_currentSetupObject);
    //    m_setupObjects.append(m_currentSetupObject);
    //    m_currentSetupObject = nullptr;
    //}
    // setupNext();
}

void SetupManager::setupFailed(const QString &msg)
{
    // Q_ASSERT(m_page);
    // // m_page->addMessage(SetupPage::Error, msg);
    // if (m_currentSetupObject) {
    //     m_setupObjects.append(m_currentSetupObject);
    //     m_currentSetupObject = nullptr;
    // }
    // rollback();
}

void SetupManager::setupInfo(const QString &msg)
{
    // Q_ASSERT(m_page);
    // m_page->addMessage(SetupPage::Info, msg);
}

void SetupManager::setupNext()
{
    // user canceled during the previous setup step
    if (m_rollbackRequested) {
        rollback();
        return;
    }

    if (!m_objectToSetup.isEmpty()) {
        m_currentSetupObject = m_objectToSetup.takeFirst();
        m_currentSetupObject->create();
    }
}

void SetupManager::rollback()
{
    const auto setupObjectsList = m_setupObjects;
    for (int i = 0; i < setupObjectsList.count(); ++i) {
        auto obj = m_setupObjects.at(i);
        if (obj) {
            obj->destroy();
            m_objectToSetup.prepend(obj);
        }
    }
    m_setupObjects.clear();
    m_rollbackRequested = false;
    Q_EMIT rollbackComplete();
}

void SetupManager::setName(const QString &name)
{
    m_name = name;
}

QString SetupManager::name() const
{
    return m_name;
}

void SetupManager::setEmail(const QString &email)
{
    m_email = email;
}

QString SetupManager::email() const
{
    return m_email;
}

void SetupManager::setPassword(const QString &password)
{
    m_password = password;
}

QString SetupManager::password() const
{
    return m_password;
}

QString SetupManager::country() const
{
    return QLocale::countryToString(QLocale().country());
}

void SetupManager::setPgpAutoEncrypt(bool autoencrypt)
{
    m_pgpAutoEncrypt = autoencrypt;
}

void SetupManager::setPgpAutoSign(bool autosign)
{
    m_pgpAutoSign = autosign;
}

void SetupManager::setKey(const GpgME::Key &key)
{
    m_key = key;
}

void SetupManager::setKeyPublishingMethod(Key::PublishingMethod method)
{
    m_keyPublishingMethod = method;
}

void SetupManager::openWallet()
{
    qWarning() << " SetupManager::openWallet() not implemented yet";
    // Remove it we need to update qt5keychain
    //    using namespace KWallet;
    //    if (Wallet::isOpen(Wallet::NetworkWallet())) {
    //        return;
    //    }

    //    Q_ASSERT(parent()->isWidgetType());
    //    m_wallet = Wallet::openWallet(Wallet::NetworkWallet(), qobject_cast<QWidget *>(parent())->effectiveWinId(), Wallet::Asynchronous);
    //    QEventLoop loop;
    //    connect(m_wallet, &KWallet::Wallet::walletOpened, &loop, &QEventLoop::quit);
    //    loop.exec();
}

bool SetupManager::personalDataAvailable() const
{
    return m_personalDataAvailable;
}

void SetupManager::setPersonalDataAvailable(bool available)
{
    m_personalDataAvailable = available;
}

IspdbHelper *SetupManager::ispDB(const QString &type)
{
    const QString t = type.toLower();
    if (t == QLatin1String("autoconfigkolabmail")) {
        return new SetupAutoconfigKolabMail(this);
    } else if (t == QLatin1String("autoconfigkolabldap")) {
        return new SetupAutoconfigKolabLdap(this);
    } else if (t == QLatin1String("autoconfigkolabfreebusy")) {
        return new SetupAutoconfigKolabFreebusy(this);
    } else if (t == QLatin1String("ispdb")) {
        return new IspdbHelper(this);
    } else {
        return new IspdbHelper(this);
    }
}

void SetupManager::requestRollback()
{
    if (m_setupObjects.isEmpty()) {
        Q_EMIT rollbackComplete();
    } else {
        m_rollbackRequested = true;
        if (!m_currentSetupObject) {
            rollback();
        }
    }
}
