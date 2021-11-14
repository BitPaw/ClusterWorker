#include "UserInterface.h"

UserInterface::UserInterface(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    connect(ui.StartButton, &QPushButton::clicked, this, &UserInterface::On_Button_Clicked);
}

void UserInterface::On_Button_Clicked() {
    ui.progressBar->setValue(50);
}