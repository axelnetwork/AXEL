// Copyright (c) 2019 The AXEL Core developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.


#include <unistd.h>
#include <stdio.h>
#include "urlrequest.h"

UrlRequest::UrlRequest()
{
    m_networkManager = new QNetworkAccessManager(this);
}

QString UrlRequest::get(QString url)
{
    QString body;
    QEventLoop loop;
    QUrl q_url = QUrl(url);
    QNetworkAccessManager *m_networkManager = new QNetworkAccessManager(this);
    QNetworkReply* reply = m_networkManager->get(QNetworkRequest(q_url));
    connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    QVariant statusCodeV =
        reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    QVariant redirectionTargetUrl =
        reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    loop.exec();
    if(reply->error() == QNetworkReply::NoError)
    {
        QByteArray bytes = reply->readAll();
        body = QString::fromUtf8(bytes);

    }
    reply->deleteLater();
    return body;
}

