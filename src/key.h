/*
    Copyright (c) 2016  Daniel Vrátil <dvratil@kde.org>

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

#ifndef KEY_H
#define KEY_H

#include "setupobject.h"

#include <QPointer>
#include <QProcess>
#include <gpgme++/key.h>

class KJob;
namespace GpgME {
class Error;
}

namespace QGpgME {
}

class Key : public SetupObject
{
    Q_OBJECT

public:
    enum PublishingMethod {
        NoPublishing,
        WKS,
        PKS
    };

    explicit Key(QObject *parent = nullptr);
    ~Key() override;

    void create() override;
    void destroy() override;

public Q_SLOTS:
    Q_SCRIPTABLE void setKey(const GpgME::Key &key);
    Q_SCRIPTABLE void setPublishingMethod(Key::PublishingMethod method);
    Q_SCRIPTABLE void setMailBox(const QString &mailbox);
    Q_SCRIPTABLE void setTransportId(int transportId);

private:
    void publishWKS();
    void publishPKS();

    void onWKSPublishingCheckDone(const GpgME::Error &error, const QByteArray &returnedData, const QByteArray &returnedError);
    void onWKSPublishingRequestCreated(const GpgME::Error &error, const QByteArray &returnedData, const QByteArray &returnedError);
    void onWKSPublishingRequestSent(KJob *job);

    void onPKSPublishingFinished(int result, QProcess::ExitStatus status);

private:
    int m_transportId = 0;
    GpgME::Key m_key;
    QString m_mailbox;
    QPointer<QObject> mJob;
    PublishingMethod m_publishingMethod = NoPublishing;
};

#endif
