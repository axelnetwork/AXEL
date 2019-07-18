// Copyright (c) 2019 The AXEL Core developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef AXEL_QT_URLREQUEST_H
#define AXEL_QT_URLREQUEST_H

#include <QString>
#include <QNetworkAccessManager>
#include <QUrl>
#include <QNetworkReply>
#include <QEventLoop>


class UrlRequest: public QObject
{
    Q_OBJECT

public:
    UrlRequest();
    ~UrlRequest() {};
    QString get(QString url);

private:
    QNetworkAccessManager *m_networkManager;
};


#endif // AXEL_QT_URLREQUEST_H
