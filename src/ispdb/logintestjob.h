// SPDX-FileCopyrightText: 2026 Alan Thouvenin <ath@enioka.com>
// SPDX-License-Identifier: LGPL-2.0-or-later

#pragma once
#include "configurationmodel.h"
#include <KCompositeJob>
#include <KIMAP/LoginJob>
#include <KSMTP/LoginJob>

class LoginTestJob : public KCompositeJob
{
    Q_OBJECT
public:
    void start() override;
    void handleSmtpConnectionError(const QString &error);
    void setConfiguration(const Configuration *configuration);
    void setPassword(const QString &password);

protected:
    void slotResult(KJob *job) override;

private:
    KIMAP::LoginJob *m_incomingLoginJob = nullptr;
    KSmtp::LoginJob *m_outgoingLoginJob = nullptr;
    const Configuration *m_configuration = nullptr;
    QString m_password;
};
