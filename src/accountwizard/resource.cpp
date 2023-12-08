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
#include <QMetaMethod>

using namespace Akonadi;

static QMetaType::Type argumentType(const QMetaObject *mo, const QString &method)
{
    QMetaMethod m;
    const int numberOfMethod(mo->methodCount());
    for (int i = 0; i < numberOfMethod; ++i) {
        const QString signature = QLatin1String(mo->method(i).methodSignature());
        if (signature.contains(method + QLatin1Char('('))) {
            m = mo->method(i);
            break;
        }
    }

    if (m.methodSignature().isEmpty()) {
        qCWarning(ACCOUNTWIZARD_LOG) << "Did not find D-Bus method: " << method << " available methods are:";
        for (int i = 0; i < numberOfMethod; ++i) {
            qCWarning(ACCOUNTWIZARD_LOG) << mo->method(i).methodSignature();
        }
        return QMetaType::UnknownType;
    }

    const QList<QByteArray> argTypes = m.parameterTypes();
    if (argTypes.count() != 1) {
        return QMetaType::UnknownType;
    }

    return static_cast<QMetaType::Type>(QMetaType::fromName(argTypes.first().constData()).id());
}

Resource::Resource(QObject *parent)
    : SetupBase{parent}
{
}

Resource::~Resource() = default;

void Resource::createResource()
{
    if (mResourceInfo.isValid()) {
        qCWarning(ACCOUNTWIZARD_LOG) << "mResourceInfo is not valid. It's a bug.";
        deleteLater();
        return;
    }
    const AgentType type = AgentManager::self()->type(mResourceInfo.typeIdentifier);
    if (!type.isValid()) {
        Q_EMIT error(i18n("Resource type '%1' is not available.", mResourceInfo.typeIdentifier));
        deleteLater();
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
                deleteLater();
                return;
            }
        }
    }

    Q_EMIT info(i18n("Creating resource instance for '%1'...", type.name()));
    auto job = new AgentInstanceCreateJob(type, this);
    connect(job, &AgentInstanceCreateJob::result, this, &Resource::instanceCreateResult);
    job->start();
}

void Resource::instanceCreateResult(KJob *job)
{
    if (job->error()) {
        Q_EMIT error(i18n("Failed to create resource instance: %1", job->errorText()));
        deleteLater();
        return;
    }
    mInstance = qobject_cast<AgentInstanceCreateJob *>(job)->instance();

    if (!mResourceInfo.settings.isEmpty()) {
        Q_EMIT info(i18n("Configuring resource instance..."));
        const auto service = ServerManager::agentServiceName(ServerManager::Resource, mInstance.identifier());
        QDBusInterface iface(service, QStringLiteral("/Settings"));
        if (!iface.isValid()) {
            Q_EMIT error(i18n("Unable to configure resource instance."));
            deleteLater();
            return;
        }

        // configure resource
        if (!mResourceInfo.name.isEmpty()) {
            mInstance.setName(mResourceInfo.name);
        }
        QMap<QString, QVariant>::const_iterator end(mResourceInfo.settings.constEnd());
        for (QMap<QString, QVariant>::const_iterator it = mResourceInfo.settings.constBegin(); it != end; ++it) {
            qCDebug(ACCOUNTWIZARD_LOG) << "Setting up " << it.key() << " for agent " << mInstance.identifier();
            const QString methodName = QStringLiteral("set%1").arg(it.key());
            QVariant arg = it.value();
            const QMetaType::Type targetType = argumentType(iface.metaObject(), methodName);
            if (arg.metaType().id() != targetType) {
                Q_EMIT error(i18n("Could not convert value of setting '%1' to required type %2.", it.key(), QLatin1String(QMetaType(targetType).name())));
                qCWarning(ACCOUNTWIZARD_LOG) << "Impossible to convert argument : " << arg;
                return;
            }
            // arg.convert(targetType);
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

    Q_EMIT finished(i18n("Resource setup completed."));
    deleteLater();
}

Resource::ResourceInfo Resource::resourceInfo() const
{
    return mResourceInfo;
}

void Resource::setResourceInfo(const ResourceInfo &newResourceInfo)
{
    mResourceInfo = newResourceInfo;
}

QDebug operator<<(QDebug d, const Resource::ResourceInfo &t)
{
    d << "name: " << t.name;
    d << "typeIdentifier: " << t.typeIdentifier;
    d << "settings: " << t.settings;
    return d;
}

bool Resource::ResourceInfo::isValid() const
{
    return !name.isEmpty() && !typeIdentifier.isEmpty() && !settings.isEmpty();
}

#include "moc_resource.cpp"
