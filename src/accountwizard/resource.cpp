/*
    SPDX-FileCopyrightText: 2023 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "resource.h"
#include "accountwizard_debug.h"

#include <KLocalizedString>

#include <Akonadi/AgentInstanceCreateJob>
#include <Akonadi/AgentManager>
#include <Akonadi/AgentType>
#include <Akonadi/ServerManager>

#include <QDBusInterface>
#include <QDBusReply>

using namespace Akonadi;
Resource::Resource(const QString &resourceType, QObject *parent)
    : QObject{parent}
    , mTypeIdentifier(resourceType)
{
}

Resource::~Resource()
{
}

void Resource::createResource()
{
    const AgentType type = AgentManager::self()->type(mTypeIdentifier);
    if (!type.isValid()) {
        Q_EMIT error(i18n("Resource type '%1' is not available.", mTypeIdentifier));
        return;
    }

    // check if unique instance already exists
    // qCDebug(ACCOUNTWIZARD_LOG) << type.capabilities();
    if (type.capabilities().contains(QLatin1String("Unique"))) {
        const Akonadi::AgentInstance::List lstAgent = AgentManager::self()->instances();
        for (const AgentInstance &instance : lstAgent) {
            // qCDebug(ACCOUNTWIZARD_LOG) << instance.type().identifier() << (instance.type() == type);
            if (instance.type() == type) {
                Q_EMIT finished(i18n("Resource '%1' is already set up.", type.name()));
                return;
            }
        }
    }

    Q_EMIT info(i18n("Creating resource instance for '%1'...", type.name()));
    auto *job = new AgentInstanceCreateJob(type, this);
    connect(job, &AgentInstanceCreateJob::result, this, &Resource::instanceCreateResult);
    job->start();
}

void Resource::instanceCreateResult(KJob *job)
{
    if (job->error()) {
        Q_EMIT error(i18n("Failed to create resource instance: %1", job->errorText()));
        return;
    }
    mInstance = qobject_cast<AgentInstanceCreateJob *>(job)->instance();
#if 0

    if (!m_settings.isEmpty()) {
        Q_EMIT info(i18n("Configuring resource instance..."));
        const auto service = ServerManager::agentServiceName(ServerManager::Resource, mInstance.identifier());
        QDBusInterface iface(service, QStringLiteral("/Settings"));
        if (!iface.isValid()) {
            Q_EMIT error(i18n("Unable to configure resource instance."));
            return;
        }

        // configure resource
        if (!mName.isEmpty()) {
            mInstance.setName(mName);
        }
        QMap<QString, QVariant>::const_iterator end(m_settings.constEnd());
        for (QMap<QString, QVariant>::const_iterator it = m_settings.constBegin(); it != end; ++it) {
            qCDebug(ACCOUNTWIZARD_LOG) << "Setting up " << it.key() << " for agent " << m_instance.identifier();
            const QString methodName = QStringLiteral("set%1").arg(it.key());
            QVariant arg = it.value();
            const QVariant::Type targetType = argumentType(iface.metaObject(), methodName);
            if (!arg.canConvert(targetType)) {
                Q_EMIT error(i18n("Could not convert value of setting '%1' to required type %2.", it.key(), QLatin1String(QVariant::typeToName(targetType))));
                qCWarning(ACCOUNTWIZARD_LOG) << "Impossible to convert argument : " << arg;
                return;
            }
            arg.convert(targetType);
            QDBusReply<void> reply = iface.call(methodName, arg);
            if (!reply.isValid()) {
                Q_EMIT error(i18n("Could not set setting '%1': %2", it.key(), reply.error().message()));
                return;
            }
        }
        QDBusReply<void> reply = iface.call(QStringLiteral("save"));
        if (!reply.isValid()) {
            Q_EMIT error(i18n("Could not save settings: %1", reply.error().message()));
            return;
        }
        mInstance.reconfigure();
    }

#endif
    Q_EMIT finished(i18n("Resource setup completed."));
}

QString Resource::name() const
{
    return mName;
}

void Resource::setName(const QString &newName)
{
    mName = newName;
}
