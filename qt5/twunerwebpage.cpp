/***************************************************************************
 *   Copyright (C) 2011~2011 by CSSlayer                                   *
 *   wengxt@gmail.com                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation, version 2 of the License.               *
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

#include "common.h"

// Qt
#include <QDebug>
#include <QProcess>
#include <QClipboard>
#include <QApplication>
#include <QNetworkRequest>
#include <QNetworkProxy>
#include <QDesktopServices>
#include <QFileDialog>
#include <QWebFrame>

#ifdef MEEGO_EDITION_HARMATTAN
#include <MMessageBox>
#endif

// Twuner
#include "twunerwebpage.h"
#include "twunerrequest.h"
#include "mainwindow.h"

TwunerWebPage::TwunerWebPage(MainWindow *window, QObject* parent) :
    QWebPage(parent)
{
    m_mainWindow = window;
    networkAccessManager()->setProxy(QNetworkProxy::DefaultProxy);
}

bool TwunerWebPage::acceptNavigationRequest(QWebFrame * frame, const QNetworkRequest & request, NavigationType type)
{
    Q_UNUSED(frame);
    Q_UNUSED(type);
    return handleUri(request.url().toString());
}

bool TwunerWebPage::handleUri(const QString& originmsg)
{
    QString msg = originmsg;
    if (msg.startsWith("twuner:")) {
        msg = msg.mid(6);
        QString type = msg.section("/", 0, 0);
        QString method = msg.section("/", 1, 1);
        if (type == "system") {
            if (method == "notify") {
                QString notify_type = QUrl::fromPercentEncoding(msg.section("/", 2, 2).toUtf8());
                QString title = QUrl::fromPercentEncoding(msg.section("/", 3, 3).toUtf8());
                QString summary = QUrl::fromPercentEncoding(msg.section("/", 4, 4).toUtf8());
                QString image = QUrl::fromPercentEncoding(msg.section("/", 5, 5).toUtf8());

                m_mainWindow->notification(notify_type, title, summary, image);
            } else if (method == "unread_alert") {
                QString number = QUrl::fromPercentEncoding(msg.section("/", 2, 2).toUtf8());
                m_mainWindow->unreadAlert(number);
            } else if (method == "load_settings") {
                QString settingString = QUrl::fromPercentEncoding(msg.section("/", 2, -1).toUtf8());
                currentFrame()->evaluateJavaScript("twuner_qt = " + settingString + ";");
                QString proxyType = currentFrame()->evaluateJavaScript("twuner_qt.proxy_type").toString();
                QNetworkProxy proxy;
                QNetworkAccessManager* nm = NULL;
#ifdef HAVE_KDE
                if (proxyType == "none") {
                    nm = new QNetworkAccessManager(this);
                }
#else
                if (proxyType == "system")
                {
                    nm = new QNetworkAccessManager(this);
                    QList<QNetworkProxy> proxies = QNetworkProxyFactory::systemProxyForQuery();
                    proxy = proxies[0];
                }
#endif
                if (proxyType == "http" || proxyType == "socks") {
                    nm = new QNetworkAccessManager(this);
                    bool proxyAuth = currentFrame()->evaluateJavaScript("twuner_qt.proxy_auth").toBool();
                    int proxyPort = currentFrame()->evaluateJavaScript("twuner_qt.proxy_port").toInt();
                    QString proxyHost = currentFrame()->evaluateJavaScript("twuner_qt.proxy_host").toString();
                    QString proxyAuthName = currentFrame()->evaluateJavaScript("twuner_qt.proxy_auth_name").toString();
                    QString proxyAuthPassword = currentFrame()->evaluateJavaScript("twuner_qt.proxy_auth_password").toString();

                    proxy = QNetworkProxy(proxyType == "socks" ? QNetworkProxy::Socks5Proxy : QNetworkProxy::HttpProxy,
                                        proxyHost,
                                        proxyPort);

                    if (proxyAuth) {
                        proxy.setUser(proxyAuthName);
                        proxy.setPassword(proxyAuthPassword);
                    }
                }
                if (proxy.type() != QNetworkProxy::NoProxy) {
                    QNetworkProxy::setApplicationProxy(proxy);
                }

                if (nm != NULL) {
                    QNetworkAccessManager* oldnm = networkAccessManager();
                    oldnm->setParent(NULL);
                    oldnm->deleteLater();
                    nm->setProxy(QNetworkProxy::DefaultProxy);
                    setNetworkAccessManager(nm);
                }
            } else if (method == "sign_in") {
                m_mainWindow->setSignIn(true);
            } else if (method == "sign_out") {
                m_mainWindow->setSignIn(false);
            }
        } else if (type == "action") {
            if (method == "search") {

            } else if (method == "choose_file") {
                QFileDialog dialog;
                dialog.setAcceptMode(QFileDialog::AcceptOpen);
                dialog.setFileMode(QFileDialog::ExistingFile);
                dialog.setNameFilter(tr("Images (*.png *.bmp *.jpg *.gif)"));
                int result = dialog.exec();
                if (result) {
                    QStringList fileNames = dialog.selectedFiles();
                    if (fileNames.size() > 0) {
                        QString callback = msg.section("/", 2, 2);
                        currentFrame()->evaluateJavaScript(QString("%1(\"%2\")").arg(callback, QUrl::fromLocalFile(fileNames[0]).toString().replace("file://", "")));
                    }
                }
            } else if (method == "save_avatar") {
            } else if (method == "log") {
                qDebug() << msg;
            } else if (method == "paste_clipboard_text") {
                triggerAction(QWebPage::Paste);
            } else if (method == "set_clipboard_text") {
                QClipboard *clipboard = QApplication::clipboard();
                if (clipboard)
                    clipboard->setText(msg.section("/", 2, -1));
            }
        } else if (type == "request") {
            QString json = QUrl::fromPercentEncoding(msg.section("/", 1, -1).toUtf8());
            currentFrame()->evaluateJavaScript(QString("twuner_qt_request_json = %1 ;").arg(json));
            QString request_uuid = currentFrame()->evaluateJavaScript(QString("twuner_qt_request_json.uuid")).toString();
            QString request_method = currentFrame()->evaluateJavaScript(QString("twuner_qt_request_json.method")).toString();
            QString request_url = currentFrame()->evaluateJavaScript(QString("twuner_qt_request_json.url")).toString();
            QMap<QString, QVariant> request_params = currentFrame()->evaluateJavaScript(QString("twuner_qt_request_json.params")).toMap();
            QMap<QString, QVariant> request_headers = currentFrame()->evaluateJavaScript(QString("twuner_qt_request_json.headers")).toMap();
            QList<QVariant> request_files = currentFrame()->evaluateJavaScript(QString("twuner_qt_request_json.files")).toList();

            TwunerRequest* request = new TwunerRequest(
                request_uuid,
                request_method,
                request_url,
                request_params,
                request_headers,
                request_files,
                userAgentForUrl(request_url),
                networkAccessManager());
            connect(request, SIGNAL(requestFinished(TwunerRequest*, QByteArray, QString, bool)), this, SLOT(requestFinished(TwunerRequest*, QByteArray, QString, bool)));
            if (!request->doRequest())
                delete request;
        }
    } else if (msg.startsWith("file://") || msg.startsWith("qrc:")) {
        return true;
    } else if (msg.startsWith("about:")) {
        return false;
    } else if (msg.startsWith("http://stat.draekko.org")) {
        return false;
    } else {
        QDesktopServices::openUrl(msg);
    }
    return false;
}

void TwunerWebPage::javaScriptAlert(QWebFrame *frame, const QString &msg)
{
    Q_UNUSED(frame);
    handleUri(msg);
}

#ifdef MEEGO_EDITION_HARMATTAN
bool TwunerWebPage::javaScriptConfirm(QWebFrame *frame, const QString &msg)
{
    Q_UNUSED(frame);
    MMessageBox *messageBox = new MMessageBox(msg, M::YesButton|M::NoButton);
    //int result = messageBox->exec(m_mainWindow);
    messageBox->appear(MSceneWindow::DestroyWhenDone);
    int result = 0;
    return (result == MDialog::Accepted);
}

#endif

void TwunerWebPage::requestFinished(TwunerRequest* request, QByteArray result, QString uuid , bool error)
{
    QString strresult = QString::fromUtf8(result);
    if (error) {
        QString scripts = QString("widget.DialogManager.alert('%1', '%2');\n"
                                  "globals.network.error_task_table['%3']('');\n"
                                 ).arg("Ooops, an Error occurred!", strresult, uuid);
        currentFrame()->evaluateJavaScript(scripts);
    } else {
        QString scripts;
        if (strresult.startsWith("[") || strresult.startsWith("{"))
            scripts = QString("globals.network.success_task_table['%1'](%2);"
                             ).arg(uuid, strresult);
        else
            scripts = QString("globals.network.success_task_table['%1']('%2');"
                             ).arg(uuid, strresult);
        currentFrame()->evaluateJavaScript(scripts);
    }
    request->deleteLater();
}
