#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_UserInterface.h"

class UserInterface : public QMainWindow
{
    Q_OBJECT

public:
    UserInterface(QWidget *parent = Q_NULLPTR);
   
public slots:
    void On_Button_Clicked();
private:
    Ui::UserInterfaceClass ui;
    
};
