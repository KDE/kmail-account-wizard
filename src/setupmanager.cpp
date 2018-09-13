/*
    Copyright (c) 2009 Volker Krause <vkrause@kde.org>

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

#include "setupmanager.h"
#include "resource.h"
#include "setuppage.h"
#include "transport.h"
#include "configfile.h"
#include "ldap.h"
#include "identity.h"
#include "setupispdb.h"
#include "setupautoconfigkolabmail.h"
#include "setupautoconfigkolabldap.h"
#include "setupautoconfigkolabfreebusy.h"
#include "key.h"

#include <kemailsettings.h>
#include <kwallet.h>
#include <KAssistantDialog>

#include <QLocale>

SetupManager::SetupManager(QWidget *parent)
    : QObject(parent)
    , m_currentSetupObject(nullptr)
    , m_page(nullptr)
    , m_wallet(nullptr)
    , m_keyPublishingMethod(Key::NoPublishing)
    , m_personalDataAvailable(false)
    , m_rollbackRequested(false)
    , m_pgpAutoSign(false)
    , m_pgpAutoEncrypt(false)
{
    KEMailSettings e;
    m_name = e.getSetting(KEMailSettings::RealName);
    m_email = e.getSetting(KEMailSettings::EmailAddress);
}

SetupManager::~SetupManager()
{
    delete m_wallet;
}

void SetupManager::setSetupPage(SetupPage *page)
{
    m_page = page;
}

QObject *SetupManager::createResource(const QString &type)
{
    return connectObject(new Resource(type, this));
}

QObject *SetupManager::createTransport(const QString &type)
{
    return connectObject(new Transport(type, this));
}

QObject *SetupManager::createConfigFile(const QString &fileName)
{
    return connectObject(new ConfigFile(fileName, this));
}

QObject *SetupManager::createLdap()
{
    return connectObject(new Ldap(this));
}

QObject *SetupManager::createIdentity()
{
    Identity *identity = new Identity(this);
    identity->setEmail(m_email);
    identity->setRealName(m_name);
    identity->setPgpAutoSign(m_pgpAutoSign);
    identity->setPgpAutoEncrypt(m_pgpAutoEncrypt);
    identity->setKey(m_key.protocol(), m_key.primaryFingerprint());
    return connectObject(identity);
}

QObject *SetupManager::createKey()
{
    Key *key = new Key(this);
    key->setKey(m_key);
    key->setMailBox(m_email);
    key->setPublishingMethod(m_keyPublishingMethod);
    return connectObject(key);
}

QList<SetupObject *> SetupManager::objectsToSetup() const
{
    return m_objectToSetup;
}

QList<SetupObject *> SetupManager::setupObjects() const
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
    if (m_keyPublishingMethod != Key::NoPublishing) {
        auto key = qobject_cast<Key *>(createKey());
        auto it = std::find_if(m_setupObjects.cbegin(), m_setupObjects.cend(),
                               [](SetupObject *obj) -> bool {
            return qobject_cast<Transport *>(obj);
        });
        if (it != m_setupObjects.cend()) {
            key->setDependsOn(*it);
        }
    }

    m_page->setStatus(i18n("Setting up account..."));
    m_page->setValid(false);
    m_page->assistantDialog()->backButton()->setEnabled(false);

    // ### FIXME this is a bad over-simplification and would need a real topological sort
    // but for current usage it is good enough
    std::stable_sort(m_objectToSetup.begin(), m_objectToSetup.end(), dependencyCompare);
    setupNext();
}

void SetupManager::setupSucceeded(const QString &msg)
{
    Q_ASSERT(m_page);
    m_page->addMessage(SetupPage::Success, msg);
    if (m_currentSetupObject) {
        Q_EMIT setupFinished(m_currentSetupObject);
        m_setupObjects.append(m_currentSetupObject);
        m_currentSetupObject = nullptr;
    }
    setupNext();
}

void SetupManager::setupFailed(const QString &msg)
{
    Q_ASSERT(m_page);
    m_page->addMessage(SetupPage::Error, msg);
    if (m_currentSetupObject) {
        m_setupObjects.append(m_currentSetupObject);
        m_currentSetupObject = nullptr;
    }
    rollback();
}

void SetupManager::setupInfo(const QString &msg)
{
    Q_ASSERT(m_page);
    m_page->addMessage(SetupPage::Info, msg);
}

void SetupManager::setupNext()
{
    // user canceled during the previous setup step
    if (m_rollbackRequested) {
        rollback();
        return;
    }

    if (m_objectToSetup.isEmpty()) {
        m_page->setStatus(i18n("Setup complete."));
        m_page->setProgress(100);
        m_page->setValid(true);
        m_page->assistantDialog()->backButton()->setEnabled(false);
    } else {
        const int setupObjectCount = m_objectToSetup.size() + m_setupObjects.size();
        const int remainingObjectCount = setupObjectCount - m_objectToSetup.size();
        m_page->setProgress((remainingObjectCount * 100) / setupObjectCount);
        m_currentSetupObject = m_objectToSetup.takeFirst();
        m_currentSetupObject->create();
    }
}

void SetupManager::rollback()
{
    m_page->setStatus(i18n("Failed to set up account, rolling back..."));
    const int setupObjectCount = m_objectToSetup.size() + m_setupObjects.size();
    int remainingObjectCount = m_setupObjects.size();
    foreach (SetupObject *obj, m_setupObjects) {
        m_page->setProgress((remainingObjectCount * 100) / setupObjectCount);
        if (obj) {
            obj->destroy();
            m_objectToSetup.prepend(obj);
        }
    }
    m_setupObjects.clear();
    m_page->setProgress(0);
    m_page->setStatus(i18n("Failed to set up account."));
    m_page->setValid(true);
    m_rollbackRequested = false;
    Q_EMIT rollbackComplete();
}

SetupObject *SetupManager::connectObject(SetupObject *obj)
{
    connect(obj, &SetupObject::finished, this, &SetupManager::setupSucceeded);
    connect(obj, &SetupObject::info, this, &SetupManager::setupInfo);
    connect(obj, &SetupObject::error, this, &SetupManager::setupFailed);
    m_objectToSetup.append(obj);
    return obj;
}

void SetupManager::setName(const QString &name)
{
    m_name = name;
}

QString SetupManager::name()
{
    return m_name;
}

void SetupManager::setEmail(const QString &email)
{
    m_email = email;
}

QString SetupManager::email()
{
    return m_email;
}

void SetupManager::setPassword(const QString &password)
{
    m_password = password;
}

QString SetupManager::password()
{
    return m_password;
}

QString SetupManager::country()
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
    using namespace KWallet;
    if (Wallet::isOpen(Wallet::NetworkWallet())) {
        return;
    }

    Q_ASSERT(parent()->isWidgetType());
    m_wallet = Wallet::openWallet(Wallet::NetworkWallet(), qobject_cast<QWidget *>(parent())->effectiveWinId(), Wallet::Asynchronous);
    QEventLoop loop;
    connect(m_wallet, &KWallet::Wallet::walletOpened, &loop, &QEventLoop::quit);
    loop.exec();
}

bool SetupManager::personalDataAvailable()
{
    return m_personalDataAvailable;
}

void SetupManager::setPersonalDataAvailable(bool available)
{
    m_personalDataAvailable = available;
}

QObject *SetupManager::ispDB(const QString &type)
{
    const QString t = type.toLower();
    if (t == QLatin1String("autoconfigkolabmail")) {
        return new SetupAutoconfigKolabMail(this);
    } else if (t == QLatin1String("autoconfigkolabldap")) {
        return new SetupAutoconfigKolabLdap(this);
    } else if (t == QLatin1String("autoconfigkolabfreebusy")) {
        return new SetupAutoconfigKolabFreebusy(this);
    } else if (t == QLatin1String("ispdb")) {
        return new SetupIspdb(this);
    } else {
        return new SetupIspdb(this);
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
