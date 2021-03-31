/*
    SPDX-FileCopyrightText: 2009 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QWidget>

class KAssistantDialog;
class KPageWidgetItem;

class Page : public QWidget
{
    Q_OBJECT
public:
    explicit Page(KAssistantDialog *parent);

    void setPageWidgetItem(KPageWidgetItem *item);

    virtual void enterPageNext();
    virtual void enterPageBack();
    virtual void leavePageNext();
    virtual void leavePageBack();
    virtual void leavePageNextRequested();
    virtual void leavePageBackRequested();

    KAssistantDialog *assistantDialog() const;

Q_SIGNALS:
    Q_SCRIPTABLE void pageEnteredNext();
    Q_SCRIPTABLE void pageEnteredBack();
    Q_SCRIPTABLE void pageLeftNext();
    Q_SCRIPTABLE void pageLeftBack();
    Q_SCRIPTABLE void leavePageNextOk();
    Q_SCRIPTABLE void leavePageBackOk();

public Q_SLOTS:
    Q_SCRIPTABLE void setValid(bool valid);
    Q_SCRIPTABLE void nextPage();

protected:
    KPageWidgetItem *m_item = nullptr;
    KAssistantDialog *m_parent = nullptr;

private:
    friend class Dialog;
    bool m_valid = false;
};

