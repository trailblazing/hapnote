/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the demonstration applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file. Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "downloadmanager.h"

#include "autosaver.h"
#include "libraries/qtSingleApplication5/qtsingleapplication.h"
#include "networkaccessmanager.h"

#include <math.h>

#include <QtCore/QMetaEnum>
#include <QtCore/QSettings>

#include <QtGui/QDesktopServices>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QFileIconProvider>

#include <QtCore/QDebug>

#include <QWebEngineSettings>
#include <QWebEngineDownloadItem>


namespace browser {

    /*!
        DownloadWidget is a widget that is displayed in the download manager list.
        It moves the data from the QWebEngineDownloadItem into the QFile as well
        as update the information/progressbar and report errors.
     */

    DownloadWidget::DownloadWidget(QWebEngineDownloadItem *download, QWidget *parent)
        : QWidget(parent)
        , _bytesreceived(0)
        , _download(download)
    {
        setupUi(this);
        QPalette p = downloadInfoLabel->palette();
        p.setColor(QPalette::Text, Qt::darkGray);
        downloadInfoLabel->setPalette(p);
        progressBar->setMaximum(0);
        connect(stopButton, SIGNAL(clicked()), this, SLOT(stop()));
        connect(openButton, SIGNAL(clicked()), this, SLOT(open()));

        if(download) {
            _file.setFile(download->path());
            _url = download->url();
        }

        init();
    }

    void DownloadWidget::init()
    {
        if(_download) {
            connect(
                //                _download.get() //
                //                _download.data()
                _download
                , &QWebEngineDownloadItem::downloadProgress, this, &DownloadWidget::downloadProgress);
            connect(
                //                _download.get() //
                //                _download.data()
                _download
                , &QWebEngineDownloadItem::finished, this, &DownloadWidget::finished);
        }

        // reset info
        downloadInfoLabel->clear();
        progressBar->setValue(0);
        getFileName();

        // start timer for the download estimation
        _downloadtime.start();
    }

    bool DownloadWidget::getFileName(bool promptForFileName)
    {
        QSettings settings;
        settings.beginGroup(QLatin1String("downloadmanager"));
        QString defaultLocation = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);

        if(_file.absoluteDir().exists())
            defaultLocation = _file.absolutePath();

        QString downloadDirectory = settings.value(QLatin1String("downloadDirectory"), defaultLocation).toString();

        if(!downloadDirectory.isEmpty())
            downloadDirectory += QLatin1Char('/');

        QString defaultFileName = QFileInfo(downloadDirectory, _file.fileName()).absoluteFilePath();
        QString fileName = defaultFileName;

        if(promptForFileName) {
            fileName = QFileDialog::getSaveFileName(this, tr("Save File"), defaultFileName);

            if(fileName.isEmpty()) {
                if(_download)
                    _download->cancel();

                fileNameLabel->setText(tr("Download canceled: %1").arg(QFileInfo(defaultFileName).fileName()));
                return false;
            }
        }

        _file.setFile(fileName);

        if(_download && _download->state() == QWebEngineDownloadItem::DownloadRequested)
            _download->setPath(_file.absoluteFilePath());

        fileNameLabel->setText(_file.fileName());
        return true;
    }

    void DownloadWidget::stop()
    {
        setUpdatesEnabled(false);
        stopButton->setEnabled(false);
        stopButton->hide();
        setUpdatesEnabled(true);

        if(_download)
            _download->cancel();

        emit statusChanged();
    }

    void DownloadWidget::open()
    {
        QUrl url = QUrl::fromLocalFile(_file.absolutePath());
        QDesktopServices::openUrl(url);
    }

    void DownloadWidget::downloadProgress(qint64 bytesReceived, qint64 bytesTotal)
    {
        _bytesreceived = bytesReceived;

        if(bytesTotal == -1) {
            progressBar->setValue(0);
            progressBar->setMaximum(0);
        } else {
            progressBar->setValue(bytesReceived);
            progressBar->setMaximum(bytesTotal);
        }

        updateInfoLabel();
    }

    void DownloadWidget::updateInfoLabel()
    {
        qint64 bytesTotal = progressBar->maximum();

        // update info label
        double speed = _bytesreceived * 1000.0 / _downloadtime.elapsed();
        double timeRemaining = ((double)(bytesTotal - _bytesreceived)) / speed;
        QString timeRemainingString = tr("seconds");

        if(timeRemaining > 60) {
            timeRemaining = timeRemaining / 60;
            timeRemainingString = tr("minutes");
        }

        timeRemaining = floor(timeRemaining);

        // When downloading the eta should never be 0
        if(timeRemaining == 0)
            timeRemaining = 1;

        QString info;

        if(!downloadedSuccessfully()) {
            QString remaining;

            if(bytesTotal != 0)
                remaining = tr("- %4 %5 remaining")
                            .arg(timeRemaining)
                            .arg(timeRemainingString);

            info = tr("%1 of %2 (%3/sec) %4")
                   .arg(dataString(_bytesreceived))
                   .arg(bytesTotal == 0 ? tr("?") : dataString(bytesTotal))
                   .arg(dataString((int)speed))
                   .arg(remaining);
        } else {
            if(_bytesreceived != bytesTotal) {
                info = tr("%1 of %2 - Stopped")
                       .arg(dataString(_bytesreceived))
                       .arg(dataString(bytesTotal));
            } else
                info = dataString(_bytesreceived);
        }

        downloadInfoLabel->setText(info);
    }

    QString DownloadWidget::dataString(int size) const
    {
        QString unit;

        if(size < 1024) {
            unit = tr("bytes");
        } else if(size < 1024 * 1024) {
            size /= 1024;
            unit = tr("kB");
        } else {
            size /= 1024 * 1024;
            unit = tr("MB");
        }

        return QString(QLatin1String("%1 %2")).arg(size).arg(unit);
    }

    bool DownloadWidget::downloading() const
    {
        return (progressBar->isVisible());
    }

    bool DownloadWidget::downloadedSuccessfully() const
    {
        bool completed = _download
                         && _download->isFinished()
                         && _download->state() == QWebEngineDownloadItem::DownloadCompleted;
        return completed || !stopButton->isVisible();
    }

    void DownloadWidget::finished()
    {
        if(_download) {
            QWebEngineDownloadItem::DownloadState state = _download->state();
            QString message;
            bool interrupted = false;

            switch(state) {
                case QWebEngineDownloadItem::DownloadRequested: // Fall-through.
                case QWebEngineDownloadItem::DownloadInProgress:
                    Q_UNREACHABLE();
                    break;

                case QWebEngineDownloadItem::DownloadCompleted:
                    break;

                case QWebEngineDownloadItem::DownloadCancelled:
                    message = QStringLiteral("Download cancelled");
                    interrupted = true;
                    break;

                case QWebEngineDownloadItem::DownloadInterrupted:
                    message = QStringLiteral("Download interrupted");
                    interrupted = true;
                    break;
            }

            if(interrupted) {
                downloadInfoLabel->setText(message);
                return;
            }
        }

        progressBar->hide();
        stopButton->setEnabled(false);
        stopButton->hide();
        updateInfoLabel();
        emit statusChanged();
    }

    /*!
        DownloadManager is a Dialog that contains a list of DownloadWidgets

        It is a basic download manager.  It only downloads the file, doesn't do BitTorrent,
        extract zipped files or anything fancy.
      */
    DownloadManager::DownloadManager(QWidget *parent)
        : QDialog(parent)
        , _autosaver(new AutoSaver(this))
        , _iconprovider(0)
        , _removepolicy(Never)
    {
        setupUi(this);
        downloadsView->setShowGrid(false);
        downloadsView->verticalHeader()->hide();
        downloadsView->horizontalHeader()->hide();
        downloadsView->setAlternatingRowColors(true);
        downloadsView->horizontalHeader()->setStretchLastSection(true);
        _model = new DownloadModel(this);
        downloadsView->setModel(_model);
        connect(cleanupButton, SIGNAL(clicked()), this, SLOT(cleanup()));
        load();
    }

    DownloadManager::~DownloadManager()
    {
        _autosaver->changeOccurred();
        _autosaver->saveIfNeccessary();

        if(_iconprovider)
            delete _iconprovider;
    }

    int DownloadManager::activeDownloads() const
    {
        int count = 0;

        for(int i = 0; i < _downloads.count(); ++i) {
            if(_downloads.at(i)->stopButton->isEnabled())
                ++count;
        }

        return count;
    }

    void DownloadManager::download(QWebEngineDownloadItem *download)
    {
        DownloadWidget *widget = new DownloadWidget(download, this);
        addItem(widget);
    }

    void DownloadManager::addItem(DownloadWidget *widget)
    {
        connect(widget, SIGNAL(statusChanged()), this, SLOT(updateRow()));
        int row = _downloads.count();
        _model->beginInsertRows(QModelIndex(), row, row);
        _downloads.append(widget);
        _model->endInsertRows();
        updateItemCount();

        if(row == 0)
            show();

        downloadsView->setIndexWidget(_model->index(row, 0), widget);
        QIcon icon = style()->standardIcon(QStyle::SP_FileIcon);
        widget->fileIcon->setPixmap(icon.pixmap(48, 48));
        downloadsView->setRowHeight(row, widget->sizeHint().height());
    }

    void DownloadManager::updateRow()
    {
        DownloadWidget *widget = qobject_cast<DownloadWidget *>(sender());
        int row = _downloads.indexOf(widget);

        if(-1 == row)
            return;

        if(!_iconprovider)
            _iconprovider = new QFileIconProvider();

        QIcon icon = _iconprovider->icon(widget->_file);

        if(icon.isNull())
            icon = style()->standardIcon(QStyle::SP_FileIcon);

        widget->fileIcon->setPixmap(icon.pixmap(48, 48));
        downloadsView->setRowHeight(row, widget->minimumSizeHint().height());

        bool remove = false;

        if(!widget->downloading()
           && QtSingleApplication::instance()->privateBrowsing())
            remove = true;

        if(widget->downloadedSuccessfully()
           && removePolicy() == DownloadManager::SuccessFullDownload) {
            remove = true;
        }

        if(remove)
            _model->removeRow(row);

        cleanupButton->setEnabled(_downloads.count() - activeDownloads() > 0);
    }

    DownloadManager::RemovePolicy DownloadManager::removePolicy() const
    {
        return _removepolicy;
    }

    void DownloadManager::setRemovePolicy(RemovePolicy policy)
    {
        if(policy == _removepolicy)
            return;

        _removepolicy = policy;
        _autosaver->changeOccurred();
    }

    void DownloadManager::save() const
    {
        QSettings settings;
        settings.beginGroup(QLatin1String("downloadmanager"));
        QMetaEnum removePolicyEnum = staticMetaObject.enumerator(staticMetaObject.indexOfEnumerator("RemovePolicy"));
        settings.setValue(QLatin1String("removeDownloadsPolicy"), QLatin1String(removePolicyEnum.valueToKey(_removepolicy)));
        settings.setValue(QLatin1String("size"), size());

        if(_removepolicy == Exit)
            return;

        for(int i = 0; i < _downloads.count(); ++i) {
            QString key = QString(QLatin1String("download_%1_")).arg(i);
            settings.setValue(key + QLatin1String("url"), _downloads[i]->_url);
            settings.setValue(key + QLatin1String("location"), _downloads[i]->_file.filePath());
            settings.setValue(key + QLatin1String("done"), _downloads[i]->downloadedSuccessfully());
        }

        int i = _downloads.count();
        QString key = QString(QLatin1String("download_%1_")).arg(i);

        while(settings.contains(key + QLatin1String("url"))) {
            settings.remove(key + QLatin1String("url"));
            settings.remove(key + QLatin1String("location"));
            settings.remove(key + QLatin1String("done"));
            key = QString(QLatin1String("download_%1_")).arg(++i);
        }
    }

    void DownloadManager::load()
    {
        QSettings settings;
        settings.beginGroup(QLatin1String("downloadmanager"));
        QSize size = settings.value(QLatin1String("size")).toSize();

        if(size.isValid())
            resize(size);

        QByteArray value = settings.value(QLatin1String("removeDownloadsPolicy"), QLatin1String("Never")).toByteArray();
        QMetaEnum removePolicyEnum = staticMetaObject.enumerator(staticMetaObject.indexOfEnumerator("RemovePolicy"));
        _removepolicy = removePolicyEnum.keyToValue(value) == -1 ?
                        Never :
                        static_cast<RemovePolicy>(removePolicyEnum.keyToValue(value));

        int i = 0;
        QString key = QString(QLatin1String("download_%1_")).arg(i);

        while(settings.contains(key + QLatin1String("url"))) {
            QUrl url = settings.value(key + QLatin1String("url")).toUrl();
            QString fileName = settings.value(key + QLatin1String("location")).toString();
            bool done = settings.value(key + QLatin1String("done"), true).toBool();

            if(done && !url.isEmpty() && !fileName.isEmpty()) {
                DownloadWidget *widget = new DownloadWidget(0, this);
                widget->_file.setFile(fileName);
                widget->fileNameLabel->setText(widget->_file.fileName());
                widget->_url = url;
                widget->stopButton->setVisible(false);
                widget->stopButton->setEnabled(false);
                widget->progressBar->hide();
                addItem(widget);
            }

            key = QString(QLatin1String("download_%1_")).arg(++i);
        }

        cleanupButton->setEnabled(_downloads.count() - activeDownloads() > 0);
    }

    void DownloadManager::cleanup()
    {
        if(_downloads.isEmpty())
            return;

        _model->removeRows(0, _downloads.count());
        updateItemCount();

        if(_downloads.isEmpty() && _iconprovider) {
            delete _iconprovider;
            _iconprovider = 0;
        }

        _autosaver->changeOccurred();
    }

    void DownloadManager::updateItemCount()
    {
        int count = _downloads.count();
        itemCount->setText(count == 1 ? tr("1 Download") : tr("%1 Downloads").arg(count));
    }

    DownloadModel::DownloadModel(DownloadManager *downloadManager, QObject *parent)
        : QAbstractListModel(parent)
        , _downloadmanager(downloadManager)
    {
    }

    QVariant DownloadModel::data(const QModelIndex &index, int role) const
    {
        if(index.row() < 0 || index.row() >= rowCount(index.parent()))
            return QVariant();

        if(role == Qt::ToolTipRole)
            if(!_downloadmanager->_downloads.at(index.row())->downloadedSuccessfully())
                return _downloadmanager->_downloads.at(index.row())->downloadInfoLabel->text();

        return QVariant();
    }

    int DownloadModel::rowCount(const QModelIndex &parent) const
    {
        return (parent.isValid()) ? 0 : _downloadmanager->_downloads.count();
    }

    bool DownloadModel::removeRows(int row, int count, const QModelIndex &parent)
    {
        if(parent.isValid())
            return false;

        int lastRow = row + count - 1;

        for(int i = lastRow; i >= row; --i) {
            if(_downloadmanager->_downloads.at(i)->downloadedSuccessfully()) {
                beginRemoveRows(parent, i, i);
                _downloadmanager->_downloads.takeAt(i)->deleteLater();
                endRemoveRows();
            }
        }

        _downloadmanager->_autosaver->changeOccurred();
        return true;
    }


}