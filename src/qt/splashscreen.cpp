// Copyright (c) 2011-2014 The Bitcoin developers
// Copyright (c) 2014-2015 The Dash developers
// Copyright (c) 2015-2017 The PIVX developers
// Copyright (c) 2019-2020 The AXEL Core developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "splashscreen.h"

#include "clientversion.h"
#include "init.h"
#include "networkstyle.h"
#include "ui_interface.h"
#include "util.h"
#include "version.h"

#ifdef ENABLE_WALLET
#include "wallet.h"
#endif

#include <QApplication>
#include <QCloseEvent>
#include <QDesktopWidget>
#include <QPainter>
#include <QSplashScreen>

SplashScreen::SplashScreen(Qt::WindowFlags f, const NetworkStyle* networkStyle) : QWidget(0, f), curAlignment(0)
{
    // set reference point, paddings
    int paddingLeft = 0;
    int paddingTop = 450;
    int titleVersionVSpace = 17;
    int titleCopyrightVSpace = 32;

    float fontFactor = 1.0;

    // define text to place
    QString titleText = tr("AXEL Wallet");
    QString versionText = QString(tr("Version %1")).arg(QString::fromStdString(FormatFullVersion()));
    QString copyrightTextBtc = QChar(0xA9) + QString(" 2009-%1 ").arg(2014) + QString(tr("The Bitcoin Core developers"));
    QString copyrightTextDash = QChar(0xA9) + QString(" 2014-%1 ").arg(2015) + QString(tr("The Dash Core developers"));
    QString copyrightTextPIVX = QChar(0xA9) + QString(" 2015-%1 ").arg(2017) + QString(tr("The PIVX Core developers"));
	QString copyrightTextBulwark = QChar(0xA9) + QString(" 2017-%1 ").arg(2018) + QString(tr("The Bulwark developers"));
	QString copyrightTextesbc = QChar(0xA9) + QString(" 2018-%1 ").arg(2019) + QString(tr("The esbcoin Core developers"));
    QString copyrightTextaxel = QChar(0xA9) + QString(" 2019-%1 ").arg(COPYRIGHT_YEAR) + QString(tr("The AXEL Wallet developers"));
    QString titleAddText = networkStyle->getTitleAddText();

    QString font = QApplication::font().toString();

    this->setWindowFlags(Qt::FramelessWindowHint);
    // load the bitmap for writing some text over it
    pixmap = networkStyle->getSplashImage();

    QPainter pixPaint(&pixmap);
    pixPaint.setPen(QColor(80, 80, 80));

    //pixPaint.setFont(QFont(font, 15 * fontFactor));
    //QFontMetrics fm = pixPaint.fontMetrics();
    //int titleTextWidth = fm.width(titleText);
    //if (titleTextWidth > 160) {
        // strange font rendering, Arial probably not found
    //    fontFactor = 0.75;
    //}

    //pixPaint.setFont(QFont(font, 15 * fontFactor));
    //fm = pixPaint.fontMetrics();
    //titleTextWidth = fm.width(titleText);
    //pixPaint.drawText(paddingLeft+172, paddingTop,titleText);

    //pixPaint.setFont(QFont(font, 10 * fontFactor));
    titleVersionVSpace = 82;
    //draw text later, after resize
    //pixPaint.drawText(paddingLeft+201, paddingTop + titleVersionVSpace, versionText);
    
    // draw copyright stuff
    //pixPaint.setPen(QColor(160, 160, 160));
    //pixPaint.setFont(QFont(font, 12 * fontFactor));
    //pixPaint.drawText(paddingLeft+141, paddingTop + titleCopyrightVSpace, copyrightTextBtc);
    //pixPaint.drawText(paddingLeft+150, paddingTop + titleCopyrightVSpace + 14, copyrightTextDash);
    //pixPaint.drawText(paddingLeft+148, paddingTop + titleCopyrightVSpace + 28, copyrightTextPIVX);
	//pixPaint.drawText(paddingLeft+155, paddingTop + titleCopyrightVSpace + 42, copyrightTextBulwark);
	//pixPaint.drawText(paddingLeft+141, paddingTop + titleCopyrightVSpace + 56, copyrightTextesbc);
    //pixPaint.drawText(paddingLeft+160, paddingTop + titleCopyrightVSpace + 70, copyrightTextaxel);

    // draw additional text if special network
    if (!titleAddText.isEmpty()) {
        QFont boldFont = QFont(font, 10 * fontFactor);
        boldFont.setWeight(QFont::Bold);
        pixPaint.setFont(boldFont);
        QFontMetrics fm = pixPaint.fontMetrics();
        int titleAddTextWidth = fm.width(titleAddText);
        pixPaint.drawText(pixmap.width() - titleAddTextWidth - 10, pixmap.height() - 25, titleAddText);
    }

    //pixPaint.end();


    // Set window title
    setWindowTitle(titleText + " " + titleAddText);

    // Resize window and move to center of desktop, disallow resizing
    QRect r(QPoint(), pixmap.size());
    resize(r.size());
    setFixedSize(r.size());

    move(QApplication::desktop()->screenGeometry().center() - r.center());
    
    //enlarge font size for hign resolution 
    QRect screenrect = QApplication::desktop()->screenGeometry();
    if(screenrect.width()>2500 && screenrect.height()>1500) 	{
#ifdef Q_OS_MAC
		pixPaint.setFont(QFont("Times",35,QFont::Bold));
#else
		pixPaint.setFont(QFont(font, 26 * fontFactor));
#endif
    }
    else if(screenrect.width()>1850 && screenrect.height()>1050)
     {
#ifdef Q_OS_MAC		
		pixPaint.setFont(QFont("Times",18,QFont::Bold));
#else
		pixPaint.setFont(QFont(font, 12 * fontFactor));
#endif
    }
    else
    {
#ifdef Q_OS_MAC			
		pixPaint.setFont(QFont("Times",14,QFont::Bold));
#else
		pixPaint.setFont(QFont(font, 11 * fontFactor));
#endif
    }
    //try to align to center
    QFontMetrics fmNew = pixPaint.fontMetrics();
    int versionTextWidth = fmNew.width(versionText);
    int versionPadding = (r.width() - versionTextWidth)/2;
    pixPaint.drawText(paddingLeft+versionPadding, paddingTop + titleVersionVSpace,versionText);
    pixPaint.end();

    subscribeToCoreSignals();
}

SplashScreen::~SplashScreen()
{
    unsubscribeFromCoreSignals();
}

void SplashScreen::slotFinish(QWidget* mainWin)
{
    Q_UNUSED(mainWin);
    hide();
}

static void InitMessage(SplashScreen* splash, const std::string& message)
{
    QMetaObject::invokeMethod(splash, "showMessage",
        Qt::QueuedConnection,
        Q_ARG(QString, QString::fromStdString(message)),
        Q_ARG(int, Qt::AlignBottom | Qt::AlignHCenter),
        Q_ARG(QColor, QColor(255, 255, 255)));
}

static void ShowProgress(SplashScreen* splash, const std::string& title, int nProgress)
{
    InitMessage(splash, title + strprintf("%d", nProgress) + "%");
}

#ifdef ENABLE_WALLET
static void ConnectWallet(SplashScreen* splash, CWallet* wallet)
{
    wallet->ShowProgress.connect(boost::bind(ShowProgress, splash, _1, _2));
}
#endif

void SplashScreen::subscribeToCoreSignals()
{
    // Connect signals to client
    uiInterface.InitMessage.connect(boost::bind(InitMessage, this, _1));
    uiInterface.ShowProgress.connect(boost::bind(ShowProgress, this, _1, _2));
#ifdef ENABLE_WALLET
    uiInterface.LoadWallet.connect(boost::bind(ConnectWallet, this, _1));
#endif
}

void SplashScreen::unsubscribeFromCoreSignals()
{
    // Disconnect signals from client
    uiInterface.InitMessage.disconnect(boost::bind(InitMessage, this, _1));
    uiInterface.ShowProgress.disconnect(boost::bind(ShowProgress, this, _1, _2));
#ifdef ENABLE_WALLET
    if (pwalletMain)
        pwalletMain->ShowProgress.disconnect(boost::bind(ShowProgress, this, _1, _2));
#endif
}

void SplashScreen::showMessage(const QString& message, int alignment, const QColor& color)
{
    curMessage = message;
    curAlignment = alignment;
    curColor = color;
    update();
}

void SplashScreen::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.drawPixmap(0, 0, pixmap);
    QRect r = rect().adjusted(5, 5, -5, -5);
    painter.setPen(curColor);
    painter.drawText(r, curAlignment, curMessage);
}

void SplashScreen::closeEvent(QCloseEvent* event)
{
    StartShutdown(); // allows an "emergency" shutdown during startup
    event->ignore();
}
