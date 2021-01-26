/*
    SPDX-FileCopyrightText: 2009 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dialog.h"
#include "global.h"

#include <controlgui.h>

#include <KAboutData>
#include <QApplication>
#include <QDebug>
#include <QUrl>

#include <KDBusService>
#include <KLocalizedString>
#include <iostream>
#include <stdio.h>

#include <KCrash>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QIcon>

int main(int argc, char **argv)
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
    QApplication app(argc, argv);
    app.setAttribute(Qt::AA_UseHighDpiPixmaps, true);
    KLocalizedString::setApplicationDomain("accountwizard");
    KCrash::initialize();
    KAboutData aboutData(QStringLiteral("accountwizard"),
                         i18n("Account Assistant"),
                         QStringLiteral("0.2"),
                         i18n("Helps setting up PIM accounts"),
                         KAboutLicense::LGPL,
                         i18n("(c) 2009-2020 the Akonadi developers"),
                         QString(),
                         QStringLiteral("https://community.kde.org/KDE_PIM/Akonadi"));
    aboutData.addAuthor(i18n("Volker Krause"), i18n("Author"), QStringLiteral("vkrause@kde.org"));
    aboutData.addAuthor(i18n("Laurent Montel"), QString(), QStringLiteral("montel@kde.org"));

    app.setOrganizationDomain(QStringLiteral("kde.org"));
    app.setWindowIcon(QIcon::fromTheme(QStringLiteral("kontact")));
    QCommandLineParser parser;
    KAboutData::setApplicationData(aboutData);
    parser.addOption(
        QCommandLineOption(QStringList() << QStringLiteral("type"), i18n("Only offer accounts that support the given type."), QStringLiteral("type")));
    parser.addOption(QCommandLineOption(QStringList() << QStringLiteral("assistant"), i18n("Run the specified assistant."), QStringLiteral("assistant")));
    parser.addOption(QCommandLineOption(QStringList() << QStringLiteral("package"),
                                        i18n("unpack fullpath on startup and launch that assistant"),
                                        QStringLiteral("fullpath")));
    parser.addOption(QCommandLineOption(QStringList() << QStringLiteral("assistants"), i18n("Returns types of assistants")));

    aboutData.setupCommandLine(&parser);
    parser.process(app);
    aboutData.processCommandLine(&parser);

    if (parser.isSet(QStringLiteral("assistants"))) {
        const QStringList lst = Global::assistants();
        std::cout << i18n("The following assistants are available:").toLocal8Bit().data() << std::endl;
        for (const QString &val : lst) {
            std::cout << "\t" << val.toLocal8Bit().constData() << std::endl;
        }
        return 0;
    }

    KDBusService service(KDBusService::Unique);

    Akonadi::ControlGui::start(nullptr);

    const QString packageArgument = parser.value(QStringLiteral("package"));
    if (!packageArgument.isEmpty()) {
        Global::setAssistant(Global::unpackAssistant(QUrl::fromLocalFile(packageArgument)));
    } else {
        Global::setAssistant(parser.value(QStringLiteral("assistant")));
    }

    QString typeValue = parser.value(QStringLiteral("type"));
    if (!typeValue.isEmpty()) {
        Global::setTypeFilter(typeValue.split(QLatin1Char(',')));
    }

    Dialog dlg(nullptr);
    dlg.show();
    // Unregister once the UI is closed, even if the app will continue running
    // and generating keys in the background.
    QObject::connect(&dlg, &Dialog::accepted, &service, &KDBusService::unregister);

    return app.exec();
}
