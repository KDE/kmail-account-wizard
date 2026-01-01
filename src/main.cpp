// SPDX-FileCopyrightText: 2009 Volker Krause <vkrause@kde.org>
// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-FileCopyrightText: 2023-2026 Laurent Montel <montel@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include <Akonadi/Control>
#include <KAboutData>
#include <KCrash>
#include <KDBusService>
#include <KLocalizedString>

#include <KIconTheme>
#include <KLocalizedQmlContext>
#include <QApplication>
#include <QCommandLineParser>
#include <QDebug>
#include <QIcon>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>

#include <KStyleManager>
using namespace Qt::Literals::StringLiterals;

int main(int argc, char **argv)
{
    KIconTheme::initTheme();
    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
    QApplication app(argc, argv);
    KLocalizedString::setApplicationDomain(QByteArrayLiteral("accountwizard"));

    // Default to org.kde.desktop style unless the user forces another style
    if (qEnvironmentVariableIsEmpty("QT_QUICK_CONTROLS_STYLE")) {
        QQuickStyle::setStyle(QStringLiteral("org.kde.desktop"));
    }

    KStyleManager::initStyle();
    KAboutData aboutData(QStringLiteral("accountwizard"),
                         i18n("Account Assistant"),
                         QStringLiteral("0.3"),
                         i18n("Helps setting up PIM accounts"),
                         KAboutLicense::LGPL,
                         i18n("© 2009-2025 the Akonadi developers"),
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

    engine.rootContext()->setContextObject(new KLocalizedQmlContext(&engine));

    engine.loadFromModule("org.kde.pim.accountwizard", "Main");

    // Exit on QML load error.
    if (engine.rootObjects().isEmpty()) {
        qWarning() << " Error during loading main.qml";
        return 1;
    }
    return app.exec();
}
