// SPDX-FileCopyrightText: 2023 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

#pragma once

#include "ispdb/serverconfiguration.h"

#include <QObject>

class QNetworkAccessManager;
class QNetworkReply;

class IspdbService : public QObject
{
    Q_OBJECT

public:
    explicit IspdbService(QObject *parent = nullptr);
    void start(const KMime::Types::AddrSpec &addrSpec);

Q_SIGNALS:
    void finished(const EmailProvider &emailProvider, const QString &messageInfo);
    void notConfigFound();
    void requestedConfigFromUrl(const QUrl &url);

private:
    enum SearchServerType {
        IspAutoConfig = 0, ///< http://autoconfig.example.com/mail/config-v1.1.xml
        IspWellKnow, ///< http://example.com/.well-known/autoconfig/mail/config-v1.1.xml
        DataBase ///< https://autoconfig.thunderbird.net/v1.1/example.com */
    };
    void requestConfig(const KMime::Types::AddrSpec &addrSpec, const SearchServerType searchServerType);
    void handleReply(QNetworkReply *const reply, const KMime::Types::AddrSpec &addrSpec, const SearchServerType searchServerType);

    QNetworkAccessManager *const mNetworkAccessManager;
};
