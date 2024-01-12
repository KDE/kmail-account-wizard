/*
    SPDX-FileCopyrightText: 2010-2024 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "identitybase.h"
#include "accountwizard_debug.h"

#include <KIdentityManagementCore/Identity>
#include <KIdentityManagementCore/IdentityManager>
#include <MailTransport/Transport>

#include <KLocalizedString>

IdentityBase::IdentityBase(QObject *parent)
    : QObject(parent)
{
    mIdentity = &KIdentityManagementCore::IdentityManager::self()->newFromScratch(QString());
    Q_ASSERT(mIdentity != nullptr);
}

void IdentityBase::create()
{
    Q_EMIT info(i18n("Setting up identity..."));

    // store identity information
    mIdentityName = identityName();
    mIdentity->setIdentityName(mIdentityName);
    createNewIdentity();
    Q_EMIT finished(i18n("Identity set up."));
}

void IdentityBase::createNewIdentity()
{
    // TODO remove it for base. Add debug output only
    // Reimplement it.
}

QString IdentityBase::identityName() const
{
    // create identity name
    QString name(mIdentityName);
    if (name.isEmpty()) {
        name = i18nc("Default name for new email accounts/identities.", "Unnamed");

        const QString idName = mIdentity->primaryEmailAddress();
        int pos = idName.indexOf(QLatin1Char('@'));
        if (pos != -1) {
            name = idName.mid(0, pos);
        }

        // Make the name a bit more human friendly
        name.replace(QLatin1Char('.'), QLatin1Char(' '));
        pos = name.indexOf(QLatin1Char(' '));
        if (pos != 0) {
            name[pos + 1] = name[pos + 1].toUpper();
        }
        name[0] = name[0].toUpper();
    }

    auto manager = KIdentityManagementCore::IdentityManager::self();
    if (!manager->isUnique(name)) {
        name = manager->makeUnique(name);
    }
    return name;
}

void IdentityBase::destroy()
{
    auto manager = KIdentityManagementCore::IdentityManager::self();
    if (!manager->removeIdentityForced(mIdentityName)) {
        qCWarning(ACCOUNTWIZARD_LOG) << " impossible to remove identity " << mIdentityName;
    }
    manager->commit();
    mIdentity = nullptr;
    Q_EMIT info(i18n("Identity removed."));
}

void IdentityBase::setIdentityName(const QString &name)
{
    mIdentityName = name;
}

QString IdentityBase::fullName() const
{
    return mIdentity->fullName();
}

void IdentityBase::setFullName(const QString &name)
{
    if (name == fullName()) {
        return;
    }
    mIdentity->setFullName(name);
    Q_EMIT fullNameChanged();
}

QString IdentityBase::organization() const
{
    return mIdentity->organization();
}

void IdentityBase::setOrganization(const QString &org)
{
    if (org == this->organization()) {
        return;
    }
    mIdentity->setOrganization(org);
    Q_EMIT organizationChanged();
}

QString IdentityBase::email() const
{
    return mIdentity->primaryEmailAddress();
}

void IdentityBase::setEmail(const QString &email)
{
    if (email == this->email()) {
        return;
    }
    mIdentity->setPrimaryEmailAddress(email);
    Q_EMIT emailChanged();
}

uint IdentityBase::uoid() const
{
    return mIdentity->uoid();
}

void IdentityBase::setTransport(MailTransport::Transport *transport)
{
    if (transport) {
        mIdentity->setTransport(QString::number(transport->id()));
    } else {
        mIdentity->setTransport({});
    }
}

QString IdentityBase::signature() const
{
    return mIdentity->signature().text();
}

void IdentityBase::setSignature(const QString &sig)
{
    if (!sig.isEmpty()) {
        const KIdentityManagementCore::Signature signature(sig);
        mIdentity->setSignature(signature);
    } else {
        mIdentity->setSignature(KIdentityManagementCore::Signature());
    }
}

void IdentityBase::setPreferredCryptoMessageFormat(const QString &format)
{
    mIdentity->setPreferredCryptoMessageFormat(format);
}

void IdentityBase::setXFace(const QString &xface)
{
    mIdentity->setXFaceEnabled(!xface.isEmpty());
    mIdentity->setXFace(xface);
}

void IdentityBase::setPgpAutoEncrypt(bool autoencrypt)
{
    mIdentity->setPgpAutoEncrypt(autoencrypt);
}

void IdentityBase::setPgpAutoSign(bool autosign)
{
    mIdentity->setPgpAutoSign(autosign);
}

void IdentityBase::setKey(GpgME::Protocol protocol, const QByteArray &fingerprint)
{
    if (fingerprint.isEmpty()) {
        mIdentity->setPGPEncryptionKey(QByteArray());
        mIdentity->setPGPSigningKey(QByteArray());
        mIdentity->setSMIMEEncryptionKey(QByteArray());
        mIdentity->setSMIMESigningKey(QByteArray());
    } else if (protocol == GpgME::OpenPGP) {
        mIdentity->setPGPSigningKey(fingerprint);
        mIdentity->setPGPEncryptionKey(fingerprint);
    } else if (protocol == GpgME::CMS) {
        mIdentity->setSMIMESigningKey(fingerprint);
        mIdentity->setSMIMEEncryptionKey(fingerprint);
    }
}

#include "moc_identitybase.cpp"
