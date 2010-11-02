/***************************************************************************
 *   Copyright (C) 2010 Kai Heitkamp                                       *
 *   dynup@ymail.com | wiibafu.codeplex.com                                *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "witools.h"

WiTools::WiTools(QObject *parent) : QObject(parent) {

}

QStandardItemModel* WiTools::getFilesGameListModel(QStandardItemModel *model, QString path) {
    emit newStatusBarMessage(tr("Loading games..."));

    QProcess filesRead;
    filesRead.start("wit", QStringList() << "LIST" << "--section" << "--recurse" << path);

    if (!filesRead.waitForFinished(-1)) {
        emit newStatusBarMessage(tr("Adding games failed!"));
        emit newLogEntry(tr("Adding games failed!\nError: %1 (%2)").arg(QString::number(filesRead.exitCode()), filesRead.errorString()));
        return model;
    }

    QByteArray bytes = filesRead.readAllStandardOutput();
    QStringList lines = QString(bytes).split("\n");

    emit newLogEntry(QString(bytes));

    double count = 0;
    QList<QStandardItem *> ids, names, titles, regions, sizes, itimes, mtimes, ctimes, atimes, filetypes, filenames;

    foreach (QString line, lines) {
        if (line.contains("total-discs=0")) {
            emit newStatusBarMessage(tr("No games found!"));
            return model;
            break;
        }
        else if (line.isEmpty())
            continue;

        if (line.startsWith("id=")) {
            ids.append(new QStandardItem(line.section("=", 1)));
            continue;
        }
        else if (line.startsWith("name=")) {
            names.append(new QStandardItem(line.section("=", 1)));
            continue;
        }
        else if (line.startsWith("title=")) {
            titles.append(new QStandardItem(line.section("=", 1)));
            continue;
        }
        else if (line.startsWith("region=")) {
            regions.append(new QStandardItem(line.section("=", 1)));
            continue;
        }
        else if (line.startsWith("size=")) {
            sizes.append(new QStandardItem(QString("%1 GB").arg(QString::number((line.section("=", 1).toDouble() / 1073741824), 'f', 2))));
            count = count + line.section("=", 1).toDouble();
            continue;
        }
        else if (line.startsWith("itime=")) {
            itimes.append(new QStandardItem(line.section("=", 1).section(" ", 1)));
            continue;
        }
        else if (line.startsWith("mtime=")) {
            mtimes.append(new QStandardItem(line.section("=", 1).section(" ", 1)));
            continue;
        }
        else if (line.startsWith("ctime=")) {
            ctimes.append(new QStandardItem(line.section("=", 1).section(" ", 1)));
            continue;
        }
        else if (line.startsWith("atime=")) {
            atimes.append(new QStandardItem(line.section("=", 1).section(" ", 1)));
            continue;
        }
        else if (line.startsWith("filetype=")) {
            filetypes.append(new QStandardItem(line.section("=", 1)));
            continue;
        }
        else if (line.startsWith("filename=")) {
            filenames.append(new QStandardItem(line.section("=", 1)));
            continue;
        }
    }

    model->appendColumn(ids);
    model->appendColumn(names);
    model->appendColumn(titles);
    model->appendColumn(regions);
    model->appendColumn(sizes);
    model->appendColumn(itimes);
    model->appendColumn(mtimes);
    model->appendColumn(ctimes);
    model->appendColumn(atimes);
    model->appendColumn(filetypes);
    model->appendColumn(filenames);

    model->setHeaderData(0, Qt::Horizontal, tr("Game ID"));
    model->setHeaderData(1, Qt::Horizontal, tr("Game title (%1 / %2 GB)").arg(QString::number(ids.count()), QString::number((count / 1073741824), 'f', 2)));
    model->setHeaderData(2, Qt::Horizontal, tr("Original title"));
    model->setHeaderData(3, Qt::Horizontal, tr("Region"));
    model->setHeaderData(4, Qt::Horizontal, tr("Size"));
    model->setHeaderData(5, Qt::Horizontal, tr("Insertion"));
    model->setHeaderData(6, Qt::Horizontal, tr("Last modification"));
    model->setHeaderData(7, Qt::Horizontal, tr("Last status change"));
    model->setHeaderData(8, Qt::Horizontal, tr("Last access"));
    model->setHeaderData(9, Qt::Horizontal, tr("Type"));
    model->setHeaderData(10, Qt::Horizontal, tr("File name"));

    emit newStatusBarMessage(tr("Ready."));

    return model;
}

QStandardItemModel* WiTools::getDVDGameListModel(QStandardItemModel *model, QString path) {
    emit newStatusBarMessage(tr("Loading disc..."));

    QProcess dvdRead;
    dvdRead.start("wit", QStringList() << "LIST-LL" << path << "--section");

    if (!dvdRead.waitForFinished(-1)) {
        emit newStatusBarMessage(tr("Loading disc failed!"));
        emit newLogEntry(tr("Loading disc failed!\nError: %1 (%2)").arg(QString::number(dvdRead.exitCode()), dvdRead.errorString()));
        return model;
    }

    QByteArray bytes = dvdRead.readAllStandardOutput();
    QStringList lines = QString(bytes).split("\n");

    emit newLogEntry(QString(bytes));

    QList<QStandardItem *> game;

    foreach (QString line, lines) {
        if (line.contains("total-discs=0")) {
            emit newStatusBarMessage(tr("No game found!"));
            return model;
            break;
        }
        else if (line.isEmpty())
            continue;

        if (line.startsWith("id=")) {
            game.append(new QStandardItem(line.section("=", 1)));
            continue;
        }
        else if (line.startsWith("name=")) {
            game.append(new QStandardItem(line.section("=", 1)));
            continue;
        }
        else if (line.startsWith("title=")) {
            game.append(new QStandardItem(line.section("=", 1)));
            continue;
        }
        else if (line.startsWith("region=")) {
            game.append(new QStandardItem(line.section("=", 1)));
            continue;
        }
        else if (line.startsWith("size=")) {
            game.append(new QStandardItem(QString("%1 GB").arg(QString::number((line.section("=", 1).toDouble() / 1073741824), 'f', 2))));
            continue;
        }
        else if (line.startsWith("itime=")) {
            if (line.section("=", 1).section(" ", 1) != "") {
                game.append(new QStandardItem(line.section("=", 1).section(" ", 1)));
            }
            else {
                game.append(new QStandardItem("--"));
            }
            continue;
        }
        else if (line.startsWith("mtime=")) {
            if (line.section("=", 1).section(" ", 1) != "") {
                game.append(new QStandardItem(line.section("=", 1).section(" ", 1)));
            }
            else {
                game.append(new QStandardItem("--"));
            }
            continue;
        }
        else if (line.startsWith("ctime=")) {
            if (line.section("=", 1).section(" ", 1) != "") {
                game.append(new QStandardItem(line.section("=", 1).section(" ", 1)));
            }
            else {
                game.append(new QStandardItem("--"));
            }
            continue;
        }
        else if (line.startsWith("atime=")) {
            if (line.section("=", 1).section(" ", 1) != "") {
                game.append(new QStandardItem(line.section("=", 1).section(" ", 1)));
            }
            else {
                game.append(new QStandardItem("--"));
            }
            continue;
        }
        else if (line.startsWith("filetype=")) {
            game.append(new QStandardItem(line.section("=", 1)));
            continue;
        }
        else if (line.startsWith("container=")) {
            game.append(new QStandardItem(line.section("=", 1)));
            continue;
        }
        else if (line.startsWith("disctype=")) {
            game.append(new QStandardItem(line.section("=", 1)));
            continue;
        }
        else if (line.startsWith("n-partitions=")) {
            game.append(new QStandardItem(line.section("=", 1)));
            continue;
        }
        else if (line.startsWith("partition-info=")) {
            game.append(new QStandardItem(line.section("=", 1)));
            continue;
        }
        else if (line.startsWith("wbfs_slot=")) {
            game.append(new QStandardItem(line.section("=", 1)));
            continue;
        }
        else if (line.startsWith("source=")) {
            game.append(new QStandardItem(line.section("=", 1)));
            continue;
        }
        else if (line.startsWith("filename=")) {
            game.append(new QStandardItem(line.section("=", 1)));
            continue;
        }
    }

    model->appendColumn(game);

    model->setHeaderData(0, Qt::Vertical, tr("Game ID:"));
    model->setHeaderData(1, Qt::Vertical, tr("Game title:"));
    model->setHeaderData(2, Qt::Vertical, tr("Original title:"));
    model->setHeaderData(3, Qt::Vertical, tr("Region:"));
    model->setHeaderData(4, Qt::Vertical, tr("Size:"));
    model->setHeaderData(5, Qt::Vertical, tr("Insertion:"));
    model->setHeaderData(6, Qt::Vertical, tr("Last modification:"));
    model->setHeaderData(7, Qt::Vertical, tr("Last status change:"));
    model->setHeaderData(8, Qt::Vertical, tr("Last access:"));
    model->setHeaderData(9, Qt::Vertical, tr("Type:"));
    model->setHeaderData(10, Qt::Vertical, tr("Container:"));
    model->setHeaderData(11, Qt::Vertical, tr("Disc type:"));
    model->setHeaderData(12, Qt::Vertical, tr("N partitions:"));
    model->setHeaderData(13, Qt::Vertical, tr("Partition info:"));
    model->setHeaderData(14, Qt::Vertical, tr("WBFS slot:"));
    model->setHeaderData(15, Qt::Vertical, tr("Source:"));
    model->setHeaderData(16, Qt::Vertical, tr("File name:"));

    emit newStatusBarMessage(tr("Ready."));

    return model;
}

QStandardItemModel* WiTools::getWBFSGameListModel(QStandardItemModel *model, QString wbfsPath) {
    QStringList arguments;
    arguments.append("LIST-L");
    if (wbfsPath.isEmpty()) {
        arguments.append("--auto");
    }
    else {
        arguments.append("--part");
        arguments.append(wbfsPath);
    }
    arguments.append("--section");

    QProcess wbfsRead;
    wbfsRead.start("wwt", arguments);

    if (!wbfsRead.waitForFinished(-1)) {
        emit newStatusBarMessage(tr("Listing games failed!"));
        emit newLogEntry(tr("Listing games failed!\nError: %1 (%2)").arg(QString::number(wbfsRead.exitCode()), wbfsRead.errorString()));
        return model;
    }

    QByteArray bytes = wbfsRead.readAllStandardOutput();
    QStringList lines = QString(bytes).split("\n");

    emit newLogEntry(QString(bytes));

    int free, total;
    QString file, usedDiscs, totalDiscs, usedMB, freeMB, totalMB;
    QList<QStandardItem *> ids, names, titles, regions, sizes, usedblocks, itimes, mtimes, ctimes, atimes, filetypes, containers, wbfsslots, filenames;

    foreach (QString line, lines) {
        if (line.contains("text=No WBFS found")) {
            emit newStatusBarMessage(tr("No WBFS partitions found!"));
            return model;
            break;
        }
        else if (line.contains("used_discs=0")) {
            emit newStatusBarMessage(tr("No games found!"));
            return model;
            break;
        }
        else if (line.isEmpty())
            continue;

        if (line.startsWith("file=")) {
            file = tr("%1").arg(line.section("=", 1));
            continue;
        }
        else if (line.startsWith("used_discs=")) {
            usedDiscs = tr("Used discs: %1").arg(line.section("=", 1));
            continue;
        }
        else if (line.startsWith("total_discs=")) {
            totalDiscs = tr("Total discs: %1").arg(line.section("=", 1));
            continue;
        }
        else if (line.startsWith("used_mib")) {
            usedMB = tr("Used MiB: %1").arg(line.section("=", 1));
            continue;
        }
        else if (line.startsWith("free_mib=")) {
            freeMB = tr("Free MiB: %1").arg(line.section("=", 1));
            free = line.section("=", 1).toInt();
            continue;
        }
        else if (line.startsWith("total_mib=")) {
            totalMB = tr("Total MiB: %1").arg(line.section("=", 1));
            total = line.section("=", 1).toInt();
            continue;
        }

        if (line.startsWith("id=")) {
            ids.append(new QStandardItem(line.section("=", 1)));
            continue;
        }
        else if (line.startsWith("name=")) {
            names.append(new QStandardItem(line.section("=", 1)));
            continue;
        }
        else if (line.startsWith("title=")) {
            titles.append(new QStandardItem(line.section("=", 1)));
            continue;
        }
        else if (line.startsWith("region=")) {
            regions.append(new QStandardItem(line.section("=", 1)));
            continue;
        }
        else if (line.startsWith("size=")) {
            sizes.append(new QStandardItem(QString("%1 GB").arg(QString::number((line.section("=", 1).toDouble() / 1073741824), 'f', 2))));
            continue;
        }
        else if (line.startsWith("used_blocks=")) {
            usedblocks.append(new QStandardItem(line.section("=", 1)));
            continue;
        }
        else if (line.startsWith("itime=")) {
            itimes.append(new QStandardItem(line.section("=", 1).section(" ", 1)));
            continue;
        }
        else if (line.startsWith("mtime=")) {
            mtimes.append(new QStandardItem(line.section("=", 1).section(" ", 1)));
            continue;
        }
        else if (line.startsWith("ctime=")) {
            ctimes.append(new QStandardItem(line.section("=", 1).section(" ", 1)));
            continue;
        }
        else if (line.startsWith("atime=")) {
            atimes.append(new QStandardItem(line.section("=", 1).section(" ", 1)));
            continue;
        }
        else if (line.startsWith("filetype=")) {
            filetypes.append(new QStandardItem(line.section("=", 1)));
            continue;
        }
        else if (line.startsWith("container=")) {
            containers.append(new QStandardItem(line.section("=", 1)));
            continue;
        }
        else if (line.startsWith("wbfs_slot=")) {
            wbfsslots.append(new QStandardItem(line.section("=", 1)));
            continue;
        }
        else if (line.startsWith("filename=")) {
            filenames.append(new QStandardItem(line.section("=", 1)));
            continue;
        }
    }

    model->appendColumn(ids);
    model->appendColumn(names);
    model->appendColumn(titles);
    model->appendColumn(regions);
    model->appendColumn(sizes);
    model->appendColumn(usedblocks);
    model->appendColumn(itimes);
    model->appendColumn(mtimes);
    model->appendColumn(ctimes);
    model->appendColumn(atimes);
    model->appendColumn(filetypes);
    model->appendColumn(containers);
    model->appendColumn(wbfsslots);
    model->appendColumn(filenames);

    model->setHeaderData(0, Qt::Horizontal, tr("Game ID"));
    model->setHeaderData(1, Qt::Horizontal, tr("Game title"));
    model->setHeaderData(2, Qt::Horizontal, tr("Original title"));
    model->setHeaderData(3, Qt::Horizontal, tr("Region"));
    model->setHeaderData(4, Qt::Horizontal, tr("Size"));
    model->setHeaderData(5, Qt::Horizontal, tr("Used blocks"));
    model->setHeaderData(6, Qt::Horizontal, tr("Insertion"));
    model->setHeaderData(7, Qt::Horizontal, tr("Last modification"));
    model->setHeaderData(8, Qt::Horizontal, tr("Last status change"));
    model->setHeaderData(9, Qt::Horizontal, tr("Last access"));
    model->setHeaderData(10, Qt::Horizontal, tr("Type"));
    model->setHeaderData(11, Qt::Horizontal, tr("Container"));
    model->setHeaderData(12, Qt::Horizontal, tr("WBFS slot"));
    model->setHeaderData(13, Qt::Horizontal, tr("File/Partition"));

    // total - free = bug fix for wwts 'used_mib'
    emit setProgressBarWBFS(0, total, total - free, QString("%1 - %2 - %3 - %4 (%p%) - %5 - %6").arg(file, usedDiscs, totalDiscs, usedMB, freeMB, totalMB));

    return model;
}

void WiTools::transferGamesToWBFS(QModelIndexList indexList, QString wbfsPath) {
    emit setMainProgressBarVisible(true);
    emit setMainProgressBar(0, "%p%");
    emit newStatusBarMessage(tr("Preparing transfer..."));

    QStringList paths;
    foreach (QModelIndex index, indexList) {
        paths.append(index.data().toString());
    }

    QStringList arguments;
    arguments.append("ADD");
    if (wbfsPath.isEmpty()) {
        arguments.append("--auto");
    }
    else {
        arguments.append("--part");
        arguments.append(wbfsPath);
    }
    arguments.append(paths);
    arguments.append("--progress");

    wwtADDProcess = new QProcess();
    qRegisterMetaType<QProcess::ExitStatus>("QProcess::ExitStatus");
    connect(wwtADDProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(transferGamesToWBFS_readyReadStandardOutput()));
    connect(wwtADDProcess, SIGNAL(readyReadStandardError()), this, SLOT(transferGamesToWBFS_readyReadStandardError()));
    connect(wwtADDProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(transferGamesToWBFS_finished(int, QProcess::ExitStatus)));

    wwtADDProcess->start("wwt", arguments);
    wwtADDProcess->waitForFinished(-1);

    emit setMainProgressBarVisible(false);
}

void WiTools::transferGamesToWBFS_readyReadStandardOutput() {
    QString line = wwtADDProcess->readAllStandardOutput().constData();

    if (line.contains("ADD")) {
        emit newStatusBarMessage(tr("Transfering game %1...").arg(line.mid(line.indexOf("ADD ") + 4, (line.lastIndexOf("]") - line.indexOf("ADD ")) - 3)));
    }
    else if (line.contains("% copied")) {
        emit setMainProgressBar(line.left(line.indexOf("%")).remove(" ").toInt(), line);
    }
    else if (line.contains("copied") && !line.contains("%")) {
        emit newLogEntry(line.remove(0, 5));
    }
    else if (line.contains("disc added.") || line.contains("discs added.")) {
        emit newLogEntry(line);
    }
}

void WiTools::transferGamesToWBFS_readyReadStandardError() {
    emit newLogEntry(wwtADDProcess->readAllStandardError().constData());
}

void WiTools::transferGamesToWBFS_finished(int exitCode, QProcess::ExitStatus exitStatus) {
    if (exitStatus == QProcess::NormalExit) {
        if (exitCode == 0) {
            emit transferGamesToWBFSsuccessfully();
        }
        else if (exitCode == 4) {
            emit transferGamesToWBFScanceled(true);
        }
    }
    else {
        emit transferGamesToWBFScanceled(false);
    }
}

void WiTools::transferGamesToWBFS_cancel() {
    wwtADDProcess->kill();
}

void WiTools::removeGamesFromWBFS(QModelIndexList indexList, QString wbfsPath) {
    QStringList ids;
    foreach (QModelIndex index, indexList) {
        ids.append(index.data().toString());
    }

    QStringList arguments;
    arguments.append("REMOVE");
    if (wbfsPath.isEmpty()) {
        arguments.append("--auto");
    }
    else {
        arguments.append("--part");
        arguments.append(wbfsPath);
    }
    arguments.append(ids);

    QProcess *wwtREMOVEProcess = new QProcess();
    qRegisterMetaType<QProcess::ExitStatus>("QProcess::ExitStatus");
    connect(wwtREMOVEProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(removeGamesFromWBFS_finished(int, QProcess::ExitStatus)));

    wwtREMOVEProcess->start("wwt", arguments);
    wwtREMOVEProcess->waitForFinished(-1);
}

void WiTools::removeGamesFromWBFS_finished(int exitCode, QProcess::ExitStatus exitStatus) {
    if (exitStatus == QProcess::NormalExit && exitCode == 0) {
        QString message(tr("Games removed successfully!"));
        emit newStatusBarMessage(message);
        emit newLogEntry(message);
        emit removeGamesFromWBFS_successfully();
    }
    else {
        QString message(tr("Games removed failed!"));
        emit newStatusBarMessage(message);
        emit newLogEntry(message);
    }
}

void WiTools::transferGamesFromWBFS(QModelIndexList indexList, QString wbfsPath, QString format, QString directory) {
    emit setMainProgressBarVisible(true);
    emit setMainProgressBar(0, "%p%");
    emit newStatusBarMessage(tr("Preparing transfer..."));

    QStringList paths;
    foreach (QModelIndex index, indexList) {
        paths.append(index.data().toString());
    }

    QStringList arguments;
    arguments.append("EXTRACT");
    if (wbfsPath.isEmpty()) {
        arguments.append("--auto");
    }
    else {
        arguments.append("--part");
        arguments.append(wbfsPath);
    }
    arguments.append(paths);
    arguments.append(QString("--").append(format));
    arguments.append("--dest");
    arguments.append(directory);
    arguments.append("--progress");

    wwtEXTRACTProcess = new QProcess();
    qRegisterMetaType<QProcess::ExitStatus>("QProcess::ExitStatus");
    connect(wwtEXTRACTProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(transferGamesFromWBFS_readyReadStandardOutput()));
    connect(wwtEXTRACTProcess, SIGNAL(readyReadStandardError()), this, SLOT(transferGamesFromWBFS_readyReadStandardError()));
    connect(wwtEXTRACTProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(transferGamesFromWBFS_finished(int, QProcess::ExitStatus)));

    wwtEXTRACTProcess->start("wwt", arguments);
    wwtEXTRACTProcess->waitForFinished(-1);

    emit setMainProgressBarVisible(false);
}

void WiTools::transferGamesFromWBFS_readyReadStandardOutput() {
    QString line = wwtEXTRACTProcess->readAllStandardOutput().constData();

    if (line.contains("EXTRACT")) {
        emit newStatusBarMessage(tr("Transfering game %1...").arg(line.mid(line.indexOf("EXTRACT") + 8, line.lastIndexOf(":") - line.indexOf("EXTRACT") - 8)));
    }
    else if (line.contains("% copied")) {
        emit setMainProgressBar(line.left(line.indexOf("%")).remove(" ").toInt(), line);
    }
    else if (line.contains("copied") && !line.contains("%")) {
        emit newLogEntry(line.remove(0, 5));
    }
    else if (line.contains("disc extracted.") || line.contains("discs extracted.")) {
        emit newLogEntry(line);
    }
}

void WiTools::transferGamesFromWBFS_readyReadStandardError() {
    emit newLogEntry(wwtEXTRACTProcess->readAllStandardError().constData());
}

void WiTools::transferGamesFromWBFS_finished(int exitCode, QProcess::ExitStatus exitStatus) {
    if (exitStatus == QProcess::NormalExit && exitCode == 0) {
        emit transferGamesFromWBFSsuccessfully();
    }
    else {
        emit transferGamesFromWBFScanceled();
    }
}

void WiTools::transferGamesFromWBFS_cancel() {
    wwtEXTRACTProcess->kill();
}

void WiTools::checkWBFS(QString wbfsPath) {
    emit newStatusBarMessage(tr("Checking WBFS..."));

    QStringList arguments;
    arguments.append("CHECK");
    if (wbfsPath.isEmpty()) {
        arguments.append("--auto");
    }
    else {
        arguments.append("--part");
        arguments.append(wbfsPath);
    }
    //arguments.append("--repair"); //TODO: Via settings
    //arguments.append("ALL"); //TODO: Via settings
    arguments.append("--long");

    QProcess wwtCHECKProcess;
    wwtCHECKProcess.start("wwt", arguments);

    if (!wwtCHECKProcess.waitForFinished(-1)) {
        emit newStatusBarMessage(tr("WBFS check failed!"));
        emit newLogEntry(tr("WBFS check failed!\nError: %1 (%2)").arg(QString::number(wwtCHECKProcess.exitCode()), wwtCHECKProcess.errorString()));
    }
    else {
        emit newStatusBarMessage(tr("WBFS check successfully!"));
    }

    emit newLogEntry(QString(wwtCHECKProcess.readAll()));
}
