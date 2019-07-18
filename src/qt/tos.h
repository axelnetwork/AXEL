// Copyright (c) 2018-2019 The AXEL Core developers

#ifndef AXEL_QT_TOS_H
#define AXEL_QT_TOS_H

#include <QDialog>

namespace Ui
{
class Tos;
}

/** Tos screen (pre-GUI startup).
  Allows the user to accept the TOS.
 */
class Tos : public QDialog
{
    Q_OBJECT

public:
    explicit Tos(QWidget* parent = 0);
    ~Tos();

    /**
     * Show TOS message. Let the user accept the TOS.
     *
     * @returns true if user accept the TOS, false if the user cancelled the selection
     * dialog.
     *
    */
    static bool showTos();
   
private:
    Ui::Tos* ui;    
};

#endif // AXEL_QT_TOS_H
