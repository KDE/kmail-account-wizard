/*
    Copyright (C) 2010-2019 Laurent Montel <montel@kde.org>

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

#include "identity.h"
#include "transport.h"

#include <kidentitymanagement/identitymanager.h>
#include <kidentitymanagement/identity.h>
#include "accountwizard_debug.h"

#include <KLocalizedString>

Identity::Identity(QObject *parent)
    : SetupObject(parent)
{
    m_identity = &KIdentityManagement::IdentityManager::self()->newFromScratch(QString());
    Q_ASSERT(m_identity != nullptr);
}

Identity::~Identity()
{
}

void Identity::create()
{
    Q_EMIT info(i18n("Setting up identity..."));

    // store identity information
    m_identity->setIdentityName(identityName());
    auto manager = KIdentityManagement::IdentityManager::self();
    manager->commit();
    if (!manager->setAsDefault(m_identity->uoid())) {
        qCWarning(ACCOUNTWIZARD_LOG) << "Impossible to find identity";
    }

    Q_EMIT finished(i18n("Identity set up."));
}

QString Identity::identityName() const
{
    // create identity name
    QString name(m_identityName);
    if (name.isEmpty()) {
        name = i18nc("Default name for new email accounts/identities.", "Unnamed");

        const QString idName = m_identity->primaryEmailAddress();
        int pos = idName.indexOf(QLatin1Char('@'));
        if (pos != -1) {
            name = idName.mid(0, pos);
        }

        // Make the name a bit more human friendly
        name.replace(QLatin1Char('.'), QLatin1Char(' '));
        pos = name.indexOf(QLatin1Char(' '));
        if (pos != 0) {
            name[ pos + 1 ] = name[ pos + 1 ].toUpper();
        }
        name[ 0 ] = name[ 0 ].toUpper();
    }

    auto manager = KIdentityManagement::IdentityManager::self();
    if (!manager->isUnique(name)) {
        name = manager->makeUnique(name);
    }
    return name;
}

void Identity::destroy()
{
    auto manager = KIdentityManagement::IdentityManager::self();
    if (!manager->removeIdentityForced(m_identity->identityName())) {
        qCWarning(ACCOUNTWIZARD_LOG) << " impossible to remove identity " << m_identity->identityName();
    }
    manager->commit();
    m_identity = nullptr;
    Q_EMIT info(i18n("Identity removed."));
}

void Identity::setIdentityName(const QString &name)
{
    m_identityName = name;
}

void Identity::setRealName(const QString &name)
{
    m_identity->setFullName(name);
}

void Identity::setOrganization(const QString &org)
{
    m_identity->setOrganization(org);
}

void Identity::setEmail(const QString &email)
{
    m_identity->setPrimaryEmailAddress(email);
}

uint Identity::uoid() const
{
    return m_identity->uoid();
}

void Identity::setTransport(QObject *transport)
{
    if (transport) {
        m_identity->setTransport(QString::number(qobject_cast<Transport *>(transport)->transportId()));
    } else {
        m_identity->setTransport(QString());
    }
    setDependsOn(qobject_cast<SetupObject *>(transport));
}

void Identity::setSignature(const QString &sig)
{
    if (!sig.isEmpty()) {
        const KIdentityManagement::Signature signature(sig);
        m_identity->setSignature(signature);
    } else {
        m_identity->setSignature(KIdentityManagement::Signature());
    }
}

void Identity::setPreferredCryptoMessageFormat(const QString &format)
{
    m_identity->setPreferredCryptoMessageFormat(format);
}

void Identity::setXFace(const QString &xface)
{
    m_identity->setXFaceEnabled(!xface.isEmpty());
    m_identity->setXFace(xface);
}

void Identity::setPgpAutoEncrypt(bool autoencrypt)
{
    m_identity->setPgpAutoEncrypt(autoencrypt);
}

void Identity::setPgpAutoSign(bool autosign)
{
    m_identity->setPgpAutoSign(autosign);
}

void Identity::setKey(GpgME::Protocol protocol, const QByteArray &fingerprint)
{
    if (fingerprint.isEmpty()) {
        m_identity->setPGPEncryptionKey(QByteArray());
        m_identity->setPGPSigningKey(QByteArray());
        m_identity->setSMIMEEncryptionKey(QByteArray());
        m_identity->setSMIMESigningKey(QByteArray());
    } else if (protocol == GpgME::OpenPGP) {
        m_identity->setPGPSigningKey(fingerprint);
        m_identity->setPGPEncryptionKey(fingerprint);
    } else if (protocol == GpgME::CMS) {
        m_identity->setSMIMESigningKey(fingerprint);
        m_identity->setSMIMEEncryptionKey(fingerprint);
    }
}
