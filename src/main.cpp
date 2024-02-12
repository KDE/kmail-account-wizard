// SPDX-FileCopyrightText: 2009 Volker Krause <vkrause@kde.org>
// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-FileCopyrightText: 2023-2024 Laurent Montel <montel.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "identitybase.h"
#include "ispdb/configurationmodel.h"
#include "manualconfiguration.h"
#include "setupmanager.h"

#include <Akonadi/Control>
#include <KAboutData>
#include <KCrash>
#include <KDBusService>
#include <KIMAP/LoginJob>
#include <KLocalizedContext>
#include <KLocalizedString>
#include <MailTransport/Transport>

#include <QApplication>
#include <QCommandLineParser>
#include <QDebug>
#include <QIcon>
#include <QQmlApplicationEngine>
#include <QUrl>
#include <QtQml>

using namespace Qt::Literals::StringLiterals;

int main(int argc, char **argv)
{
    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
    QApplication app(argc, argv);
    KLocalizedString::setApplicationDomain(QByteArrayLiteral("accountwizard"));
    KCrash::initialize();
    KAboutData aboutData(QStringLiteral("accountwizard"),
                         i18n("Account Assistant"),
                         QStringLiteral("0.3"),
                         i18n("Helps setting up PIM accounts"),
                         KAboutLicense::LGPL,
                         i18n("(c) 2009-2024 the Akonadi developers"),
                         QString(),
                         QStringLiteral("https://community.kde.org/KDE_PIM/Akonadi"));
    aboutData.addAuthor(i18n("Volker Krause"), i18n("Author"), QStringLiteral("vkrause@kde.org"));
    aboutData.addAuthor(i18n("Laurent Montel"), i18n("Maintainer"), QStringLiteral("montel@kde.org"));
    aboutData.addAuthor(i18n("Carl Schwan"), i18n("Maintainer"), QStringLiteral("carl@carlschwan.eu"));
    aboutData.setProductName(QByteArrayLiteral("Akonadi/Account Wizard"));
    app.setOrganizationDomain(QStringLiteral("kde.org"));
    app.setWindowIcon(QIcon::fromTheme(QStringLiteral("kontact")));
    QCommandLineParser parser;
    KAboutData::setApplicationData(aboutData);

    aboutData.setupCommandLine(&parser);
    parser.process(app);
    aboutData.processCommandLine(&parser);

    KDBusService service(KDBusService::Unique);

    if (!Akonadi::Control::start()) {
        qApp->exit(-1);
        return 1;
    }

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextObject(new KLocalizedContext(&engine));

    // SetupManager setupManager;
    // qmlRegisterSingletonInstance("org.kde.pim.accountwizard", 1, 0, "SetupManager", &setupManager);
    // qRegisterMetaType<ConfigurationModel *>("ConfigurationModel *");
    // qmlRegisterType<ManualConfiguration>("org.kde.pim.accountwizard", 1, 0, "ManualConfiguration");
    // qmlRegisterUncreatableType<KIMAP::LoginJob>("org.kde.pim.accountwizard", 1, 0, "LoginJob", u"Enum"_s);
    // qmlRegisterUncreatableType<MailTransport::Transport>("org.kde.pim.accountwizard", 1, 0, "Transport", u"Enum"_s);
    engine.loadFromModule("org.kde.pim.accountwizard", "Main");
    // Exit on QML load error.
    if (engine.rootObjects().isEmpty()) {
        qWarning() << " Error during loading main.qml";
        return 1;
    }
    return app.exec();
}
