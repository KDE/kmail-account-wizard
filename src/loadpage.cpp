/*
    SPDX-FileCopyrightText: 2009 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "loadpage.h"
#include "global.h"
#include <KAssistantDialog>
#include <KConfig>
#include <KConfigGroup>
#include <kross/core/action.h>
#include <QFile>
#include <KLocalizedString>

LoadPage::LoadPage(KAssistantDialog *parent)
    : Page(parent)
    , m_action(nullptr)
{
    ui.setupUi(this);
    setValid(false);
}

void LoadPage::enterPageNext()
{
    setValid(false);
    // FIXME: deletion seems to delete the exported objects as well, killing the entire wizard...
    //delete m_action;
    m_action = nullptr;
    Q_EMIT aboutToStart();

    const KConfig f(Global::assistant());
    KConfigGroup grp(&f, "Wizard");
    const QString scriptFile = grp.readEntry("Script", QString());
    if (scriptFile.isEmpty()) {
        ui.statusLabel->setText(i18n("No script specified in '%1'.", Global::assistant()));
        return;
    }
    if (!QFile::exists(Global::assistantBasePath() + scriptFile)) {
        ui.statusLabel->setText(i18n("Unable to load assistant: File '%1' does not exist.", Global::assistantBasePath() + scriptFile));
        return;
    }
    ui.statusLabel->setText(i18n("Loading script '%1'...", Global::assistantBasePath() + scriptFile));

    m_action = new Kross::Action(this, QStringLiteral("AccountWizard"));
    typedef QPair<QObject *, QString> ObjectStringPair;
    for (const ObjectStringPair &exportedObject : qAsConst(m_exportedObjects)) {
        m_action->addQObject(exportedObject.first, exportedObject.second);
    }

    if (!m_action->setFile(Global::assistantBasePath() + scriptFile)) {
        ui.statusLabel->setText(i18n("Failed to load script: '%1'.", m_action->errorMessage()));
        return;
    }

    KConfigGroup grpTranslate(&f, "Translate");
    const QString poFileName = grpTranslate.readEntry("Filename");

    if (!poFileName.isEmpty()) {
        Global::setPoFileName(poFileName);
        m_action->trigger();
    }

    m_parent->next();
}

void LoadPage::enterPageBack()
{
    // TODO: if we are the first page, call enterPageNext(), hm, can we get here then at all?
    m_parent->back();
}

void LoadPage::exportObject(QObject *object, const QString &name)
{
    m_exportedObjects.push_back(qMakePair(object, name));
}
