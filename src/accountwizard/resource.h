/*
    SPDX-FileCopyrightText: 2023 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <Akonadi/AgentInstance>
#include <QObject>
class KJob;
class Resource : public QObject
{
    Q_OBJECT
public:
    explicit Resource(const QString &resourceType, QObject *parent = nullptr);
    ~Resource() override;

    void createResource();

Q_SIGNALS:
    void error(const QString &msg);
    void info(const QString &msg);
    void finished(const QString &msg);

private:
    const QString mTypeIdentifier;
    void instanceCreateResult(KJob *job);
    Akonadi::AgentInstance mInstance;
};
