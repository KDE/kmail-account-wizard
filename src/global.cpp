/*
    SPDX-FileCopyrightText: 2009 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "global.h"

#include "accountwizard_debug.h"
#include <KIO/CopyJob>
#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>

class GlobalPrivate
{
public:
    QStringList filter;
    QString assistant;
    QString poFileName;
};

Q_GLOBAL_STATIC(GlobalPrivate, sInstance)

QString Global::assistant()
{
    return sInstance->assistant;
}

QStringList Global::assistants()
{
    QStringList list;
    const QStringList dirs =
        QStandardPaths::locateAll(QStandardPaths::GenericDataLocation, QStringLiteral("akonadi/accountwizard/"), QStandardPaths::LocateDirectory);
    for (const QString &dir : dirs) {
        const QStringList directories = QDir(dir).entryList(QDir::AllDirs);
        for (const QString &directory : directories) {
            const QString fullPath = dir + QLatin1Char('/') + directory;
            const QStringList fileNames = QDir(fullPath).entryList(QStringList() << QStringLiteral("*.desktop"));
            list.reserve(fileNames.count());
            for (const QString &file : fileNames) {
                list.append(fullPath + QLatin1Char('/') + file);
            }
        }
    }
    QStringList lstAssistants;
    lstAssistants.reserve(list.count());
    for (const QString &entry : std::as_const(list)) {
        const QFileInfo info(entry);
        const QDir dir(info.absolutePath());
        lstAssistants.append(dir.dirName());
    }
    return lstAssistants;
}

void Global::setAssistant(const QString &assistant)
{
    const QFileInfo info(assistant);
    if (info.isAbsolute()) {
        sInstance->assistant = assistant;
        return;
    }

    QStringList list;
    const QStringList dirs =
        QStandardPaths::locateAll(QStandardPaths::GenericDataLocation, QStringLiteral("akonadi/accountwizard/"), QStandardPaths::LocateDirectory);
    for (const QString &dir : dirs) {
        const QStringList directories = QDir(dir).entryList(QDir::AllDirs);
        for (const QString &directory : directories) {
            const QString fullPath = dir + QLatin1Char('/') + directory;
            const QStringList fileNames = QDir(fullPath).entryList(QStringList() << QStringLiteral("*.desktop"));
            list.reserve(fileNames.count());
            for (const QString &file : fileNames) {
                list.append(fullPath + QLatin1Char('/') + file);
            }
        }
    }
    for (const QString &entry : std::as_const(list)) {
        const QFileInfo info(entry);
        const QDir dir(info.absolutePath());
        if (dir.dirName() == assistant) {
            sInstance->assistant = entry;
            return;
        }
    }

    sInstance->assistant.clear();
}

QStringList Global::typeFilter()
{
    return sInstance->filter;
}

void Global::setTypeFilter(const QStringList &filter)
{
    sInstance->filter = filter;
}

QString Global::assistantBasePath()
{
    const QFileInfo info(assistant());
    if (info.isAbsolute()) {
        return info.absolutePath() + QDir::separator();
    }
    return QString();
}

QString Global::unpackAssistant(const QUrl &remotePackageUrl)
{
    QString localPackageFile;
    if (remotePackageUrl.scheme() == QLatin1String("file")) {
        localPackageFile = remotePackageUrl.path();
    } else {
        QString remoteFileName = QFileInfo(remotePackageUrl.path()).fileName();
        localPackageFile = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + QLatin1String("/accountwizard/") + remoteFileName;
        KIO::Job *job = KIO::copy(remotePackageUrl, QUrl::fromLocalFile(localPackageFile), KIO::Overwrite | KIO::HideProgressInfo);
        qCDebug(ACCOUNTWIZARD_LOG) << "downloading remote URL" << remotePackageUrl << "to" << localPackageFile;
        if (!job->exec()) {
            return QString();
        }
    }

    const QUrl file(QLatin1String("tar://") + localPackageFile);
    const QFileInfo fi(localPackageFile);
    const QString assistant = fi.baseName();
    const QString dest = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + QLatin1Char('/');
    QDir().mkpath(dest + file.fileName());
    KIO::Job *getJob = KIO::copy(file, QUrl::fromLocalFile(dest), KIO::Overwrite | KIO::HideProgressInfo);
    if (getJob->exec()) {
        qCDebug(ACCOUNTWIZARD_LOG) << "worked, unpacked in " << dest;
        return dest + file.fileName() + QLatin1Char('/') + assistant + QLatin1Char('/') + assistant + QLatin1String(".desktop");
    } else {
        qCDebug(ACCOUNTWIZARD_LOG) << "failed" << getJob->errorString();
        return QString();
    }
}

void Global::setPoFileName(const QString &poFileName)
{
    sInstance->poFileName = poFileName;
}

QString Global::poFileName()
{
    return sInstance->poFileName;
}
