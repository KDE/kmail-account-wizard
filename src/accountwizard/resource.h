/*
    SPDX-FileCopyrightText: 2023 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "libaccountwizard_private_export.h"
#include "setupbase.h"
#include <Akonadi/AgentInstance>
#include <QDebug>
#include <QMap>
#include <QObject>

class KJob;
class LIBACCOUNTWIZARD_TESTS_EXPORT Resource : public SetupBase
{
    Q_OBJECT
public:
    struct LIBACCOUNTWIZARD_TESTS_EXPORT ResourceInfo {
        QString name;
        QString typeIdentifier;
        QMap<QString, QVariant> settings;
    };

    explicit Resource(const QString &resourceType, QObject *parent = nullptr);
    ~Resource() override;

    [[nodiscard]] QString name() const;
    void setName(const QString &newName);

    [[nodiscard]] QMap<QString, QVariant> settings() const;
    void setSettings(const QMap<QString, QVariant> &newSettings);

    void createResource();

private:
    void instanceCreateResult(KJob *job);
    QMap<QString, QVariant> mSettings;
    Akonadi::AgentInstance mInstance;
    const QString mTypeIdentifier;
    QString mName;
};
QDebug operator<<(QDebug d, const Resource::ResourceInfo &t);
