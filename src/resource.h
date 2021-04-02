/*
    SPDX-FileCopyrightText: 2009 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "setupobject.h"
#include <Akonadi/AgentInstance>
#include <QMap>

class KJob;

class Resource : public SetupObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY name)
public:
    explicit Resource(const QString &type, QObject *parent = nullptr);
    ~Resource() = default;

    void create() override;
    void destroy() override;
    void edit();

    QString name() const;
    void setName(const QString &name);

public Q_SLOTS:
    Q_SCRIPTABLE void setOption(const QString &key, const QVariant &value);
    Q_SCRIPTABLE Q_REQUIRED_RESULT QString identifier();
    Q_SCRIPTABLE void reconfigure();
    Q_SCRIPTABLE void setEditMode(const bool editMode);

Q_SIGNALS:
    void nameChanged();

private:
    void instanceCreateResult(KJob *job);

    const QString m_typeIdentifier;
    QString m_name;
    QMap<QString, QVariant> m_settings;
    Akonadi::AgentInstance m_instance;

    bool m_editMode = false;
};
