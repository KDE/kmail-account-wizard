/*
    SPDX-FileCopyrightText: 2023-2024 Laurent Montel <montel.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "setupbase.h"
#include <Akonadi/AgentInstance>
#include <QDebug>
#include <QMap>
#include <QObject>

class KJob;
class Resource : public SetupBase
{
    Q_OBJECT
public:
    struct ResourceInfo {
        QString name;
        QString typeIdentifier;
        QMap<QString, QVariant> settings;
        [[nodiscard]] bool isValid() const;
    };

    explicit Resource(QObject *parent = nullptr);
    ~Resource() override;

    void createResource();

    [[nodiscard]] ResourceInfo resourceInfo() const;
    void setResourceInfo(const ResourceInfo &newResourceInfo);

private:
    void instanceCreateResult(KJob *job);
    Akonadi::AgentInstance mInstance;
    ResourceInfo mResourceInfo;
};

Q_DECLARE_TYPEINFO(Resource::ResourceInfo, Q_RELOCATABLE_TYPE);
QDebug operator<<(QDebug d, const Resource::ResourceInfo &t);
