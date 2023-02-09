// SPDX-FileCopyrightText: 2009 Volker Krause <vkrause@kde.org>
// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

#pragma once

#include "setupobject.h"
#include <MailTransport/Transport>

/// Configure a mail transport object
class Transport : public SetupObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString host READ host WRITE setHost NOTIFY hostChanged)
    Q_PROPERTY(int port READ port WRITE setPort NOTIFY portChanged)
    Q_PROPERTY(QString username READ username WRITE setUsername NOTIFY usernameChanged)
    Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged)
    Q_PROPERTY(QString encryption READ encryption WRITE setEncryption NOTIFY encryptionChanged)
    Q_PROPERTY(QString authenticationType READ authenticationType WRITE setAuthenticationType NOTIFY authenticationTypeChanged)
    Q_PROPERTY(bool editMode READ editMode WRITE setEditMode NOTIFY editModeChanged)
public:
    explicit Transport(const QString &type, QObject *parent = nullptr);
    void create() override;
    void destroy() override;
    void edit();

    int transportId() const;

    QString name() const;
    QString host() const;
    int port() const;
    QString username() const;
    QString password() const;
    QString encryption() const;
    QString authenticationType() const;
    bool editMode() const;
    void setName(const QString &name);
    void setHost(const QString &host);
    void setPort(int port);
    void setUsername(const QString &user);
    void setPassword(const QString &password);
    void setEncryption(const QString &encryption);
    void setAuthenticationType(const QString &authType);
    void setEditMode(bool editMode);

Q_SIGNALS:
    void nameChanged();
    void hostChanged();
    void portChanged();
    void usernameChanged();
    void passwordChanged();
    void encryptionChanged();
    void authenticationTypeChanged();
    void editModeChanged();

private:
    int m_transportId = -1;
    QString m_name;
    QString m_host;
    int m_port = -1;
    QString m_user;
    QString m_password;
    MailTransport::Transport::EnumEncryption::type m_encr;
    MailTransport::Transport::EnumAuthenticationType::type m_auth;
    QString m_encrStr;
    QString m_authStr;

    bool m_editMode = false;
};
