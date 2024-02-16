/*
    SPDX-FileCopyrightText: 2023-2024 Laurent Montel <montel.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "resource.h"
#include "accountwizard_debug.h"
#include "consolelog.h"

#include <KLocalizedString>

#include <Akonadi/AgentInstanceCreateJob>
#include <Akonadi/AgentManager>
#include <Akonadi/AgentType>
#include <Akonadi/ServerManager>
#include <MailTransport/TransportManager>

#include <QDBusInterface>
#include <QDBusReply>
#include <QMetaMethod>

using namespace Akonadi;
using namespace Qt::Literals::StringLiterals;

static QMetaType::Type argumentType(const QMetaObject *mo, const QString &method)
{
    QMetaMethod m;
    const int numberOfMethod(mo->methodCount());
    for (int i = 0; i < numberOfMethod; ++i) {
        const QString signature = QLatin1StringView(mo->method(i).methodSignature());
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

Resource::Resource(ConsoleLog *consoleLog, QObject *parent)
    : QObject{parent}
    , mConsoleLog(consoleLog)
{
}

Resource::~Resource() = default;

void Resource::createResource()
{
    if (!mResourceInfo.isValid()) {
        qCWarning(ACCOUNTWIZARD_LOG) << "mResourceInfo is not valid. It's a bug.";
        qCWarning(ACCOUNTWIZARD_LOG) << " mResourceInfo " << mResourceInfo;
        deleteLater();
        return;
    }
    const AgentType type = AgentManager::self()->type(mResourceInfo.typeIdentifier);
    if (!type.isValid()) {
        mConsoleLog->error(i18n("Resource type '%1' is not available.", mResourceInfo.typeIdentifier));
        deleteLater();
        return;
    }

    // By default imap/kolab/pop3 can't be unique. Keep it only for be safe.
    // check if unique instance already exists
    // qCDebug(ACCOUNTWIZARD_LOG) << type.capabilities();
    if (type.capabilities().contains(QLatin1StringView("Unique"))) {
        const Akonadi::AgentInstance::List lstAgent = AgentManager::self()->instances();
        for (const AgentInstance &instance : lstAgent) {
            // qCDebug(ACCOUNTWIZARD_LOG) << instance.type().identifier() << (instance.type() == type);
            if (instance.type() == type) {
                mConsoleLog->success(i18n("Resource '%1' is already set up.", type.name()));
                deleteLater();
                return;
            }
        }
    }

    mConsoleLog->info(i18n("Creating resource instance for '%1'...", type.name()));
    auto job = new AgentInstanceCreateJob(type, this);
    connect(job, &AgentInstanceCreateJob::result, this, &Resource::instanceCreateResult);
    job->start();
}

void Resource::instanceCreateResult(KJob *job)
{
    if (job->error()) {
        mConsoleLog->error(i18n("Failed to create resource instance: %1", job->errorText()));
        deleteLater();
        return;
    }
    mInstance = qobject_cast<AgentInstanceCreateJob *>(job)->instance();

    if (!mResourceInfo.settings.isEmpty()) {
        mConsoleLog->info(i18n("Configuring resource instance..."));
        const auto service = ServerManager::agentServiceName(ServerManager::Resource, mInstance.identifier());
        QDBusInterface iface(service, QStringLiteral("/Settings"));
        if (!iface.isValid()) {
            mConsoleLog->error(i18n("Unable to configure resource instance."));
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
            const QMetaType targetType = QMetaType(argumentType(iface.metaObject(), methodName));
            if (arg.metaType().id() != targetType.id()) {
                if (QMetaType::canConvert(arg.metaType(), targetType)) {
                    arg.convert(targetType);
                } else {
                    mConsoleLog->error(i18n("Could not convert value of setting '%1' with type %2 to required type %3.",
                                            it.key(),
                                            QLatin1StringView(arg.metaType().name()),
                                            QLatin1StringView(targetType.name())));

                    qCWarning(ACCOUNTWIZARD_LOG)
                        << "Impossible to convert argument : " << arg
                        << QStringLiteral("Could not convert value of setting '%1' to required type %2.").arg(it.key(), QLatin1StringView(targetType.name()));
                    return;
                }
            }
            QDBusReply<void> reply = iface.call(methodName, arg);
            if (!reply.isValid()) {
                mConsoleLog->error(i18n("Could not set setting '%1': %2", it.key(), reply.error().message()));
                return;
            }
        }
        QDBusReply<void> reply = iface.call(QStringLiteral("save"));
        if (!reply.isValid()) {
            mConsoleLog->error(i18n("Could not save settings: %1", reply.error().message()));
            return;
        }
        mInstance.reconfigure();
    }

    QString resourceLogEntryText = u"<h3>"_s + i18nc("log entry content", "Resource setup completed: %1", mResourceInfo.name) + u"</h3>"_s;

    resourceLogEntryText += u"<ul>"_s;
    resourceLogEntryText += u"<li><b>%1</b> %2</li>"_s.arg(i18nc("log entry content", "Resouce type:"), mResourceInfo.typeIdentifier);
    QMap<QString, QVariant>::const_iterator end(mResourceInfo.settings.constEnd());
    for (QMap<QString, QVariant>::const_iterator it = mResourceInfo.settings.constBegin(); it != end; ++it) {
        QString value = it.value().toString();
        if (it.key() == u"Authentication"_s) {
            value = QLatin1String(QMetaEnum::fromType<MailTransport::Transport::EnumAuthenticationType>().key(it.value().toInt()));
        } else if (it.key() == u"Safety"_s) {
            value = QLatin1String(QMetaEnum::fromType<MailTransport::Transport::EnumEncryption>().key(it.value().toInt()));
        } else if (it.key() == u"Password"_s) {
            if (value.isEmpty()) {
                value = i18nc("Indication that the password set for the resource is empty", "Empty");
            } else {
                value = u"•••••••••••"_s;
            }
        }
        resourceLogEntryText += u"<li><b>%1</b> %2</li>"_s.arg(i18nc("label", "%1:", it.key()), value);
    }
    resourceLogEntryText += u"</ul>"_s;
    mConsoleLog->success(resourceLogEntryText);

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
