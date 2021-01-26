/*
    SPDX-FileCopyrightText: 2009 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dynamicpage.h"

#include "accountwizard_debug.h"

#include "global.h"
#include <KLocalizedTranslator>
#include <QBoxLayout>
#include <QCoreApplication>
#include <QFile>
#include <QScrollArea>
#include <QUiLoader>

DynamicPage::DynamicPage(const QString &uiFile, KAssistantDialog *parent)
    : Page(parent)
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    QWidget *pageParent = this;

    QUiLoader loader;
    loader.setLanguageChangeEnabled(true);
    QFile file(uiFile);
    if (file.open(QFile::ReadOnly)) {
        qCDebug(ACCOUNTWIZARD_LOG) << uiFile;
        m_dynamicWidget = loader.load(&file, pageParent);
        file.close();
        if (!Global::poFileName().isEmpty()) {
            auto *translator = new KLocalizedTranslator(this);
            QCoreApplication::instance()->installTranslator(translator);
            translator->setTranslationDomain(Global::poFileName());

            translator->addContextToMonitor(m_dynamicWidget->objectName());
            QEvent le(QEvent::LanguageChange);
            QCoreApplication::sendEvent(m_dynamicWidget, &le);
        }
    } else {
        qCDebug(ACCOUNTWIZARD_LOG) << "Unable to open: " << uiFile;
    }

    if (m_dynamicWidget) {
        layout->addWidget(m_dynamicWidget);
    }

    setValid(true);
}

QObject *DynamicPage::widget() const
{
    return m_dynamicWidget;
}
