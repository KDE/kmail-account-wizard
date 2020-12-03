/*
    SPDX-FileCopyrightText: 2010 Omat Holding B.V. <info@omat.nl>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "ispdb.h"
#include "../accountwizard_debug.h"

#include <KAboutData>

#include <KLocalizedString>
#include <QIcon>
#include <QApplication>
#include <KAboutData>
#include <QCommandLineParser>
#include <QCommandLineOption>

QString socketTypeToStr(Ispdb::socketType socketType)
{
    QString enc = QStringLiteral("None");
    if (socketType == Ispdb::SSL) {
        enc = QStringLiteral("SSL");
    } else if (socketType == Ispdb::StartTLS) {
        enc = QStringLiteral("TLS");
    }
    return enc;
}

QString authTypeToStr(Ispdb::authType authType)
{
    QString auth = QStringLiteral("unknown");
    switch (authType) {
    case Ispdb::Plain:
        auth = QStringLiteral("plain");
        break;
    case Ispdb::CramMD5:
        auth = QStringLiteral("CramMD5");
        break;
    case Ispdb::NTLM:
        auth = QStringLiteral("NTLM");
        break;
    case Ispdb::GSSAPI:
        auth = QStringLiteral("GSSAPI");
        break;
    case Ispdb::ClientIP:
        auth = QStringLiteral("ClientIP");
        break;
    case Ispdb::NoAuth:
        auth = QStringLiteral("NoAuth");
        break;
    case Ispdb::Basic:
        auth = QStringLiteral("Basic");
        break;
    case Ispdb::OAuth2:
        auth = QStringLiteral("OAuth2");
        break;
    }
    return auth;
}

int main(int argc, char **argv)
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication app(argc, argv);
    app.setAttribute(Qt::AA_UseHighDpiPixmaps, true);
    KAboutData aboutData(QStringLiteral("ispdb"),
                         i18n("ISPDB Assistant"),
                         QStringLiteral("0.1"),
                         i18n("Helps setting up PIM accounts"),
                         KAboutLicense::LGPL,
                         i18n("(c) 2010 Omat Holding B.V."),
                         QString(),
                         QStringLiteral("https://community.kde.org/KDE_PIM/Akonadi"));
    aboutData.addAuthor(i18n("Tom Albers"), i18n("Author"), QStringLiteral("toma@kde.org"));

    app.setWindowIcon(QIcon::fromTheme(QStringLiteral("akonadi")));
    QCommandLineParser parser;
    KAboutData::setApplicationData(aboutData);
    aboutData.setupCommandLine(&parser);
    parser.addOption(QCommandLineOption(QStringList() << QStringLiteral("email"), i18n("Tries to fetch the settings for that email address"), QStringLiteral("emailaddress")));

    parser.process(app);
    aboutData.processCommandLine(&parser);

    QString email(QStringLiteral("blablabla@gmail.com"));
    const QString argEmail = parser.value(QStringLiteral("email"));
    if (!argEmail.isEmpty()) {
        email = argEmail;
    }

    QEventLoop loop;
    Ispdb ispdb;
    ispdb.setEmail(email);

    loop.connect(&ispdb, SIGNAL(finished(bool)), SLOT(quit()));

    ispdb.start();

    loop.exec();
    qCDebug(ACCOUNTWIZARD_LOG) << "Domains" << ispdb.relevantDomains();
    qCDebug(ACCOUNTWIZARD_LOG) << "Name" << ispdb.name(Ispdb::Long) << "(" << ispdb.name(Ispdb::Short) << ")";
    qCDebug(ACCOUNTWIZARD_LOG) << "Imap servers:";
    const auto imapServers = ispdb.imapServers();
    for (const Server &s : imapServers) {
        qCDebug(ACCOUNTWIZARD_LOG) << "\thostname:" << s.hostname
                                   << "- port:" << s.port
                                   << "- encryption:" << socketTypeToStr(s.socketType)
                                   << "- username:" << s.username
                                   << "- authentication:" << authTypeToStr(s.authentication);
    }
    qCDebug(ACCOUNTWIZARD_LOG) << "pop3 servers:";
    const auto pop3Servers = ispdb.pop3Servers();
    for (const Server &s : pop3Servers) {
        qCDebug(ACCOUNTWIZARD_LOG) << "\thostname:" << s.hostname
                                   << "- port:" << s.port
                                   << "- encryption:" << socketTypeToStr(s.socketType)
                                   << "- username:" << s.username
                                   << "- authentication:" << authTypeToStr(s.authentication);
    }
    qCDebug(ACCOUNTWIZARD_LOG) << "smtp servers:";
    const auto smtpServers = ispdb.smtpServers();
    for (const Server &s : smtpServers) {
        qCDebug(ACCOUNTWIZARD_LOG) << "\thostname:" << s.hostname
                                   << "- port:" << s.port
                                   << "- encryption:" << socketTypeToStr(s.socketType)
                                   << "- username:" << s.username
                                   << "- authentication:" << authTypeToStr(s.authentication);
    }

    return 0;
}
