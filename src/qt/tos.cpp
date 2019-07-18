// Copyright (c) 2018-2019 The AXEL Core developers


#include "tos.h"
#include "ui_tos.h"

#include "guiutil.h"

#include "util.h"

#include <QSettings>
#include <QPushButton>
#include <QFile>

QString TosInfo()
{
    QFile tosFile(":tos/axel_tos");
    if(!tosFile.exists())
    {
            qDebug("Agreement file not exist");
            return "<center><p><b>A X E L&nbsp;&nbsp;&nbsp;W A L L E T&nbsp;&nbsp;&nbsp;U S E R&nbsp;&nbsp;&nbsp;A G R E E M E N T</b></p></center>Please read the policies or notices on AXEL.org and AXEL.Networks (<a href='https://www.axel.org/'>https://www.axel.org/</a> ; <a href='https://axel.network/'>https://axel.network/</a> website(s) and our Privacy Policy located at <a href='https://www.axel.network/privacy-policy/'>https://www.axel.network/privacy-policy/</a>.";
    }
    if(!tosFile.open(QIODevice::ReadOnly))
    {
            qDebug("Agreement file open failed");
            return "<center><p><b>A X E L&nbsp;&nbsp;&nbsp;W A L L E T&nbsp;&nbsp;&nbsp;U S E R&nbsp;&nbsp;&nbsp;A G R E E M E N T</b></p></center>Please read the policies or notices on AXEL.org and AXEL.Networks (<a href='https://www.axel.org/'>https://www.axel.org/</a> ; <a href='https://axel.network/'>https://axel.network/</a> website(s) and our Privacy Policy located at <a href='https://www.axel.network/privacy-policy/'>https://www.axel.network/privacy-policy/</a>.";
    }

    return QString::fromUtf8(tosFile.readAll());  
}


Tos::Tos(QWidget* parent) : QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint),
                                ui(new Ui::Tos)
{
    	ui->setupUi(this);  
	
        /// HTML-format tos message from the htm file
        QString tosInfo = TosInfo();

        ui->scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

	ui->tosMessage->setText(tosInfo);

        ui->tosMessage->moveCursor(QTextCursor::Start);
        
        ui->scrollArea->setVisible(false);
        
	ui->okButton->button(QDialogButtonBox::Ok)->setText("Accept");  
}

Tos::~Tos()
{
    delete ui;    
}

bool Tos::showTos()
{
    QSettings settings;

    if (settings.contains("fTosAccept"))
	return true;
        
    Tos tos;
    
    if (!tos.exec()) {
          /* Cancel clicked */
            return false;
    }

    settings.setValue("fTosAccept", "true");
    return true;
}

