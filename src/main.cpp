/*
    Copyright (c) 2009 Volker Krause <vkrause@kde.org>

    This library is free software; you can redistribute it and/or modify it
    under the terms of the GNU Library General Public License as published by
    the Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    This library is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
    License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to the
    Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
    02110-1301, USA.
*/

#include "dialog.h"
#include "global.h"

#include <controlgui.h>

#include <KAboutData>
#include <QApplication>
#include <QUrl>
#include <QDebug>

#include <KDBusService>
#include <KLocalizedString>
#include <stdio.h>
#include <iostream>

#include <KCrash>
#include <QCommandLineParser>
#include <QCommandLineOption>
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
                         i18n("(c) 2009-2019 the Akonadi developers"),
                         QString(),
                         QStringLiteral("https://community.kde.org/KDE_PIM/Akonadi"));
    aboutData.addAuthor(i18n("Volker Krause"), i18n("Author"), QStringLiteral("vkrause@kde.org"));
    aboutData.addAuthor(i18n("Laurent Montel"), QString(), QStringLiteral("montel@kde.org"));

    app.setOrganizationDomain(QStringLiteral("kde.org"));
    app.setWindowIcon(QIcon::fromTheme(QStringLiteral("kontact")));
    QCommandLineParser parser;
    KAboutData::setApplicationData(aboutData);
    parser.addOption(QCommandLineOption(QStringList() <<  QStringLiteral("type"), i18n("Only offer accounts that support the given type."), QStringLiteral("type")));
    parser.addOption(QCommandLineOption(QStringList() <<  QStringLiteral("assistant"), i18n("Run the specified assistant."), QStringLiteral("assistant")));
    parser.addOption(QCommandLineOption(QStringList() <<  QStringLiteral("package"), i18n("unpack fullpath on startup and launch that assistant"), QStringLiteral("fullpath")));
    parser.addOption(QCommandLineOption(QStringList() <<  QStringLiteral("assistants"), i18n("Returns types of assistants")));


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
