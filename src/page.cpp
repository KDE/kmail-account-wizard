/*
    SPDX-FileCopyrightText: 2009 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "page.h"
#include <KAssistantDialog>
#include <KPageWidgetModel>

Page::Page(KAssistantDialog *parent)
    : QWidget(parent)
    , m_parent(parent)
{
}

void Page::setPageWidgetItem(KPageWidgetItem *item)
{
    m_item = item;
    m_parent->setValid(m_item, m_valid);
}

void Page::setValid(bool valid)
{
    if (!m_item) {
        m_valid = valid;
    } else {
        m_parent->setValid(m_item, valid);
    }
}

void Page::nextPage()
{
    m_parent->next();
}

KAssistantDialog *Page::assistantDialog() const
{
    return m_parent;
}

void Page::enterPageBack()
{
}

void Page::enterPageNext()
{
}

void Page::leavePageBack()
{
}

void Page::leavePageNext()
{
}

void Page::leavePageBackRequested()
{
    Q_EMIT leavePageBackOk();
}

void Page::leavePageNextRequested()
{
    Q_EMIT leavePageNextOk();
}
