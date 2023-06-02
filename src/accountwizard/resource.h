/*
    SPDX-FileCopyrightText: 2023 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <Akonadi/AgentInstance>
#include <QMap>
#include <QObject>
class KJob;
class Resource : public QObject
{
    Q_OBJECT
public:
    explicit Resource(const QString &resourceType, QObject *parent = nullptr);
    ~Resource() override;

    Q_REQUIRED_RESULT QString name() const;
    void setName(const QString &newName);

    Q_REQUIRED_RESULT QMap<QString, QVariant> settings() const;
    void setSettings(const QMap<QString, QVariant> &newSettings);

    void createResource();

Q_SIGNALS:
    void error(const QString &msg);
    void info(const QString &msg);
    void finished(const QString &msg);

private:
    void instanceCreateResult(KJob *job);
    QMap<QString, QVariant> mSettings;
    Akonadi::AgentInstance mInstance;
    const QString mTypeIdentifier;
    QString mName;
};
