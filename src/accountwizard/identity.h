/*
    SPDX-FileCopyrightText: 2010-2022 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once
#include "libaccountwizard_export.h"
#include <QObject>
#include <gpgme++/global.h>

namespace KIdentityManagementCore
{
class Identity;
}

namespace MailTransport
{
class Transport;
}

/// QObject-wrapper around a KIdentityManagement
///
/// Allow to configure an account
class LIBACCOUNTWIZARD_EXPORT Identity : public QObject
{
    Q_OBJECT

    /// This property holds the identity name
    Q_PROPERTY(QString identityName READ identityName WRITE setIdentityName NOTIFY identityNameChanged)

    /// This property holds the real name of the user.
    Q_PROPERTY(QString fullName READ fullName WRITE setFullName NOTIFY fullNameChanged)

    /// This property holds the email of the user.
    Q_PROPERTY(QString email READ email WRITE setEmail NOTIFY emailChanged)

    /// This property holds the organization of the user.
    Q_PROPERTY(QString organization READ organization WRITE setOrganization NOTIFY organizationChanged)

    /// This property holds the signature of the user.
    Q_PROPERTY(QString signature READ signature WRITE setSignature NOTIFY signatureChanged)

public:
    explicit Identity(QObject *parent = nullptr);

    Q_INVOKABLE void create();
    Q_INVOKABLE void destroy();

    Q_REQUIRED_RESULT QString identityName() const;
    void setIdentityName(const QString &name);

    Q_REQUIRED_RESULT QString fullName() const;
    void setFullName(const QString &name);

    Q_REQUIRED_RESULT QString email() const;
    void setEmail(const QString &email);

    Q_REQUIRED_RESULT QString organization() const;
    void setOrganization(const QString &org);

    Q_REQUIRED_RESULT QString signature() const;
    void setSignature(const QString &sig);

    Q_REQUIRED_RESULT uint uoid() const;
    void setTransport(MailTransport::Transport *transport);
    void setPreferredCryptoMessageFormat(const QString &format);
    void setXFace(const QString &xface);
    void setPgpAutoSign(bool autosign);
    void setPgpAutoEncrypt(bool autoencrypt);
    void setKey(GpgME::Protocol protocol, const QByteArray &fingerprint);

Q_SIGNALS:
    void info(const QString &message);
    void finished(const QString &message);
    void identityNameChanged();
    void fullNameChanged();
    void emailChanged();
    void organizationChanged();
    void signatureChanged();

private:
    QString m_identityName;
    KIdentityManagementCore::Identity *m_identity = nullptr;
};
