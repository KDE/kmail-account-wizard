// SPDX-FileCopyrightText: 2026 Alan Thouvenin <ath@enioka.com>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "logintestjob.h"
#include <KIMAP/Session>
#include <KSMTP/Session>

#include "accountwizard_debug.h"

static KIMAP::LoginJob::EncryptionMode fromEncryptionToImap(const MailTransport::Transport::EnumEncryption encryption)
{
    switch (encryption) {
    case (MailTransport::TransportBase::SSL):
        return KIMAP::LoginJob::SSLorTLS;
    case (MailTransport::TransportBase::TLS):
        return KIMAP::LoginJob::STARTTLS;
    case (MailTransport::TransportBase::None):
    default:
        return KIMAP::LoginJob::Unencrypted;
    }
}

static std::optional<KIMAP::LoginJob::AuthenticationMode> fromAuthModeToImap(MailTransport::Transport::EnumAuthenticationType authenticationType)
{
    switch (authenticationType) {
    case MailTransport::Transport::LOGIN:
        return KIMAP::LoginJob::Login;
    case MailTransport::Transport::PLAIN:
        return KIMAP::LoginJob::Plain;
    case MailTransport::Transport::CRAM_MD5:
        return KIMAP::LoginJob::CramMD5;
    case MailTransport::Transport::DIGEST_MD5:
        return KIMAP::LoginJob::DigestMD5;
    case MailTransport::Transport::GSSAPI:
        return KIMAP::LoginJob::GSSAPI;
    case MailTransport::Transport::NTLM:
        return KIMAP::LoginJob::NTLM;
    case MailTransport::Transport::CLEAR:
        return KIMAP::LoginJob::ClearText;
    case MailTransport::Transport::ANONYMOUS:
        return KIMAP::LoginJob::Anonymous;
    case MailTransport::Transport::XOAUTH2:
        return KIMAP::LoginJob::XOAuth2;
    case MailTransport::Transport::APOP:
    default:
        return {};
    }
}

static KSmtp::Session::EncryptionMode fromEncryptionToSmtp(const MailTransport::Transport::EnumEncryption encryption)
{
    switch (encryption) {
    case (MailTransport::TransportBase::SSL):
        return KSmtp::Session::TLS;
    case (MailTransport::TransportBase::TLS):
        return KSmtp::Session::STARTTLS;
    case (MailTransport::TransportBase::None):
    default:
        return KSmtp::Session::Unencrypted;
    }
}

static std::optional<KSmtp::LoginJob::AuthMode> fromAuthModeToSmtp(MailTransport::Transport::EnumAuthenticationType authenticationType)
{
    switch (authenticationType) {
    case MailTransport::Transport::LOGIN:
        return KSmtp::LoginJob::Login;
    case MailTransport::Transport::PLAIN:
    case MailTransport::Transport::CLEAR:
        return KSmtp::LoginJob::Plain;
    case MailTransport::Transport::CRAM_MD5:
        return KSmtp::LoginJob::CramMD5;
    case MailTransport::Transport::DIGEST_MD5:
        return KSmtp::LoginJob::DigestMD5;
    case MailTransport::Transport::GSSAPI:
        return KSmtp::LoginJob::GSSAPI;
    case MailTransport::Transport::NTLM:
        return KSmtp::LoginJob::NTLM;
    case MailTransport::Transport::ANONYMOUS:
        return KSmtp::LoginJob::Anonymous;
    case MailTransport::Transport::XOAUTH2:
        return KSmtp::LoginJob::XOAuth2;
    case MailTransport::Transport::APOP:
    default:
        return {};
    }
}

void LoginTestJob::handleSmtpConnectionError(const QString &error)
{
    setErrorText(error);
    setError(1); // TODO create an enum
    slotResult(m_outgoingLoginJob);
}

void LoginTestJob::start()
{
    if (m_configuration->incoming.type == Server::IMAP) {
        auto *imapSession = new KIMAP::Session(m_configuration->incoming.hostname, m_configuration->incoming.port);

        m_incomingLoginJob = new KIMAP::LoginJob(imapSession);
        m_incomingLoginJob->setUserName(m_configuration->incoming.username);
        m_incomingLoginJob->setPassword(m_password);
        m_incomingLoginJob->setEncryptionMode(fromEncryptionToImap(m_configuration->incoming.socketType));
        m_incomingLoginJob->setAuthenticationMode(fromAuthModeToImap(m_configuration->incoming.authType).value());

        addSubjob(m_incomingLoginJob);
        m_incomingLoginJob->start();
    } else {
        // TODO handle POP3
    }

    if (m_configuration->outgoing) {
        const auto &outgoing = *m_configuration->outgoing;
        auto *smtpSession = new KSmtp::Session(outgoing.hostname, outgoing.port, this);
        smtpSession->setEncryptionMode(fromEncryptionToSmtp(outgoing.socketType));

        m_outgoingLoginJob = new KSmtp::LoginJob(smtpSession);
        m_outgoingLoginJob->setUserName(outgoing.username);
        m_outgoingLoginJob->setPassword(m_password);
        m_outgoingLoginJob->setPreferedAuthMode(fromAuthModeToSmtp(outgoing.authType).value());

        addSubjob(m_outgoingLoginJob);
        connect(smtpSession, &KSmtp::Session::connectionError, this, &LoginTestJob::handleSmtpConnectionError);
        connect(smtpSession, &KSmtp::Session::stateChanged, this, [this, smtpSession](const KSmtp::Session::State state) {
            if (state == KSmtp::Session::Ready) {
                disconnect(smtpSession, &KSmtp::Session::connectionError, this, &LoginTestJob::handleSmtpConnectionError);
                m_outgoingLoginJob->start();
            }
        });
        smtpSession->open();
    }
}

void LoginTestJob::slotResult(KJob *job)
{
    if (job->error() && !error()) {
        setError(job->error());
        setErrorText(job->errorString());
        // Don't stop the job now since we need to close the sessions.
    }

    if (job == m_incomingLoginJob) {
        m_incomingLoginJob->session()->close();
        m_incomingLoginJob->session()->deleteLater();
        m_incomingLoginJob->deleteLater();
    }

    if (job == m_outgoingLoginJob) {
        m_outgoingLoginJob->session()->quit();
        m_outgoingLoginJob->session()->deleteLater();
        m_outgoingLoginJob->deleteLater();
    }

    removeSubjob(job);
    if (!hasSubjobs()) {
        emitResult();
    }
}

void LoginTestJob::setConfiguration(const Configuration *configuration)
{
    m_configuration = configuration;
}
void LoginTestJob::setPassword(const QString &password)
{
    m_password = password;
}
