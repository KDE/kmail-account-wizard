/*
    SPDX-FileCopyrightText: 2009 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once
#include "libaccountwizard_export.h"
#include "setupobject.h"
#include <Akonadi/AgentInstance>
#include <QMap>

class KJob;

class LIBACCOUNTWIZARD_EXPORT Resource : public SetupObject
{
    Q_OBJECT
public:
    explicit Resource(const QString &type, QObject *parent = nullptr);
    ~Resource() = default;

    void create() override;
    void destroy() override;
    void edit();

public Q_SLOTS:
    Q_SCRIPTABLE void setName(const QString &name);
    Q_SCRIPTABLE void setOption(const QString &key, const QVariant &value);
    Q_SCRIPTABLE [[nodiscard]] QString identifier();
    Q_SCRIPTABLE void reconfigure();
    Q_SCRIPTABLE void setEditMode(const bool editMode);

private:
    void instanceCreateResult(KJob *job);

    const QString m_typeIdentifier;
    QString m_name;
    QMap<QString, QVariant> m_settings;
    Akonadi::AgentInstance m_instance;

    bool m_editMode = false;
};
