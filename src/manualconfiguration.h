/*
    SPDX-FileCopyrightText: 2023 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/
#pragma once

#include <QObject>

class ManualConfiguration : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString incomingHostName READ incomingHostName WRITE setIncomingHostName NOTIFY incomingHostNameChanged FINAL)
    Q_PROPERTY(int incomingPort READ incomingPort WRITE setIncomingPort NOTIFY incomingPortChanged FINAL)

public:
    explicit ManualConfiguration(QObject *parent = nullptr);
    ~ManualConfiguration() override;

    Q_REQUIRED_RESULT QString incomingHostName() const;
    void setIncomingHostName(const QString &newIncomingHostName);

    Q_REQUIRED_RESULT int incomingPort() const;
    void setIncomingPort(int newPort);

Q_SIGNALS:
    void incomingHostNameChanged();
    void incomingPortChanged();

private:
    QString mIncomingHostName;
    int mIncomingPort = -1;
};
