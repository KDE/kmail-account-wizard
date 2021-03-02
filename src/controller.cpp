// SDPX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
// SPDX-LicenseRed: LGPL-2.0-or-later

#include "controller.h"
#include <KLocalizedContext>
#include <KPackage/PackageLoader>
#include <QQmlComponent>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickItem>

Controller::Controller(QObject *parent)
    : QObject(parent)
{
    m_contextObj = new KLocalizedContext(this);
}

void Controller::setWizardItem(QQuickItem *wizardItem)
{
    if (m_wizardItem == wizardItem) {
        return;
    }

    m_wizardItem = wizardItem;
    Q_EMIT wizardItemChanged();
}

QString Controller::wizardId() const
{
    return m_wizardId;
}

void Controller::setWizardId(const QString &wizardId)
{
    if (m_wizardId == wizardId) {
        return;
    }

    m_wizardId = wizardId;
    Q_EMIT wizardIdChanged();

    m_wizardPackage = KPackage::PackageLoader::self()->loadPackage(QStringLiteral("AccountWizard/Wizard"), wizardId);
    if (!m_wizardPackage.isValid()) {
        return;
    }
}

QString Controller::qmlPath() const
{
    if (m_wizardPackage.isValid()) {
        return m_wizardPackage.filePath("ui", QStringLiteral("main.qml"));
    } else {
        return QString();
    }
}

QQuickItem *Controller::wizardItem()
{
    return createGui(m_wizardPackage.filePath("ui", QStringLiteral("main.qml")));
}

QQuickItem *Controller::createGui(const QString &qmlPath)
{
    QQmlEngine *engine = qmlEngine(this);
    QQmlComponent *component = new QQmlComponent(engine, QUrl(qmlPath), nullptr);
    if (component->status() != QQmlComponent::Ready) {
        qCritical() << "Error creating component:";
        for (auto err : component->errors()) {
            qWarning() << err.toString();
        }
        component->deleteLater();
        return nullptr;
    }

    QObject *guiObject = component->create();
    QQuickItem *gui = qobject_cast<QQuickItem *>(guiObject);
    if (!gui) {
        qWarning() << "ERROR: QML gui" << guiObject << "not a QQuickItem instance";
        guiObject->deleteLater();
        return nullptr;
    }
    gui->setParent(this);
    return gui;
}
