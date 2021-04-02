// SPDX-FileCopyrightText: 2014 Sandro Knauß <knauss@kolabsys.com>
// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

#pragma once

#include "setupobject.h"

class Identity;
class Ispdb;
class Transport;
class Resource;

class IspdbHelper : public SetupObject
{
    Q_OBJECT
    // Initialize IspdbHelper
    Q_PROPERTY(QString email READ email WRITE setEmail NOTIFY emailChanged)
    Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged)

    // Information fetched from Ispdb
    Q_PROPERTY(QString shortName READ shortName NOTIFY ispdbFinished)
    Q_PROPERTY(QString longName READ longName NOTIFY ispdbFinished)
    Q_PROPERTY(QStringList relevantDomains READ relevantDomains NOTIFY ispdbFinished)
    Q_PROPERTY(int countImapServers READ countImapServers NOTIFY ispdbFinished)
    Q_PROPERTY(int countSmtpServers READ countSmtpServers NOTIFY ispdbFinished)
    Q_PROPERTY(int countIdentities READ countIdentities NOTIFY ispdbFinished)
    Q_PROPERTY(int defaultIdentity READ defaultIdentity NOTIFY ispdbFinished)
public:
    explicit IspdbHelper(QObject *parent = nullptr);
    IspdbHelper(QObject *parent, Ispdb *ispdb);
    ~IspdbHelper() override = default;

    void create() override;
    void destroy() override;

    QString shortName() const;
    QString longName() const;
    QString email() const;
    void setEmail(const QString &email);
    QString password() const;
    void setPassword(const QString &password);

    QStringList relevantDomains() const;

    Q_INVOKABLE void fillImapServer(int i, Resource *transport) const;
    int countImapServers() const;

    Q_INVOKABLE void fillSmtpServer(int i, Transport *transport) const;
    int countSmtpServers() const;

    Q_INVOKABLE void fillIdentity(int i, Identity *identity) const;
    int countIdentities() const;
    int defaultIdentity() const;

    Q_INVOKABLE void start();

Q_SIGNALS:
    void ispdbFinished(bool);
    void emailChanged();
    void passwordChanged();

protected Q_SLOTS:
    void onIspdbFinished(bool);

protected:
    Ispdb *mIspdb;
};
