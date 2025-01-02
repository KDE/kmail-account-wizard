/*
    SPDX-FileCopyrightText: 2023-2025 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <Akonadi/AgentInstance>
#include <QDebug>
#include <QMap>
#include <QObject>

class KJob;
class ConsoleLog;

class Resource : public QObject
{
    Q_OBJECT
public:
    struct ResourceInfo {
        QString name;
        QString typeIdentifier;
        QMap<QString, QVariant> settings;
        [[nodiscard]] bool isValid() const;
    };

    explicit Resource(ConsoleLog *consoleLog, QObject *parent = nullptr);
    ~Resource() override;

    void createResource();

    [[nodiscard]] ResourceInfo resourceInfo() const;
    void setResourceInfo(const ResourceInfo &newResourceInfo);

private:
    void instanceCreateResult(KJob *job);
    Akonadi::AgentInstance mInstance;
    ResourceInfo mResourceInfo;
    ConsoleLog *const mConsoleLog;
};

Q_DECLARE_TYPEINFO(Resource::ResourceInfo, Q_RELOCATABLE_TYPE);
QDebug operator<<(QDebug d, const Resource::ResourceInfo &t);
