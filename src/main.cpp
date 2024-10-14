// SPDX-FileCopyrightText: 2009 Volker Krause <vkrause@kde.org>
// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-FileCopyrightText: 2023-2024 Laurent Montel <montel.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include <Akonadi/Control>
#include <KAboutData>
#include <KCrash>
#include <KDBusService>
#include <KLocalizedString>

#include <QApplication>
#include <QCommandLineParser>
#include <QDebug>
#include <QIcon>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#if KI18N_VERSION >= QT_VERSION_CHECK(6, 8, 0)
#include <KLocalizedQmlContext>
#endif
#define HAVE_KICONTHEME __has_include(<KIconTheme>)
#if HAVE_KICONTHEME
#include <KIconTheme>
#endif

#define HAVE_STYLE_MANAGER __has_include(<KStyleManager>)
#if HAVE_STYLE_MANAGER
#include <KStyleManager>
#endif
using namespace Qt::Literals::StringLiterals;

int main(int argc, char **argv)
{
#if HAVE_KICONTHEME
    KIconTheme::initTheme();
#endif
    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
    QApplication app(argc, argv);
    KLocalizedString::setApplicationDomain(QByteArrayLiteral("accountwizard"));
#if HAVE_STYLE_MANAGER
    KStyleManager::initStyle();
#else // !HAVE_STYLE_MANAGER
#if defined(Q_OS_MACOS) || defined(Q_OS_WIN)
    QApplication::setStyle(QStringLiteral("breeze"));
#endif // defined(Q_OS_MACOS) || defined(Q_OS_WIN)
#endif // HAVE_STYLE_MANAGER
    KAboutData aboutData(QStringLiteral("accountwizard"),
                         i18n("Account Assistant"),
                         QStringLiteral("0.3"),
                         i18n("Helps setting up PIM accounts"),
                         KAboutLicense::LGPL,
                         i18n("(c) 2009-2024 the Akonadi developers"),
                         QString(),
                         QStringLiteral("https://community.kde.org/KDE_PIM/Akonadi"));
    aboutData.addAuthor(i18nc("@info:credit", "Volker Krause"), i18n("Author"), QStringLiteral("vkrause@kde.org"));
    aboutData.addAuthor(i18nc("@info:credit", "Laurent Montel"), i18n("Maintainer"), QStringLiteral("montel@kde.org"));
    aboutData.addAuthor(i18nc("@info:credit", "Carl Schwan"), i18n("Maintainer"), QStringLiteral("carl@carlschwan.eu"));
    aboutData.setProductName(QByteArrayLiteral("Akonadi/Account Wizard"));
    app.setWindowIcon(QIcon::fromTheme(QStringLiteral("kontact")));
    QCommandLineParser parser;
    KAboutData::setApplicationData(aboutData);
    KCrash::initialize();

    aboutData.setupCommandLine(&parser);
    parser.process(app);
    aboutData.processCommandLine(&parser);

    KDBusService service(KDBusService::Unique);

    if (!Akonadi::Control::start()) {
        qApp->exit(-1);
        return 1;
    }

    QQmlApplicationEngine engine;

#if KI18N_VERSION < QT_VERSION_CHECK(6, 8, 0)
    engine.rootContext()->setContextObject(new KLocalizedContext(&engine));
#else
    engine.rootContext()->setContextObject(new KLocalizedQmlContext(&engine));
#endif

    engine.loadFromModule("org.kde.pim.accountwizard", "Main");

    // Exit on QML load error.
    if (engine.rootObjects().isEmpty()) {
        qWarning() << " Error during loading main.qml";
        return 1;
    }
    return app.exec();
}
