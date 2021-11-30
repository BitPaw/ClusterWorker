#include "UserInterface.h"
#include <Network/Server.h>


UserInterface::UserInterface(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    connect(ui.StartButton, &QPushButton::clicked, this, &UserInterface::On_Button_Clicked);
}

void UserInterface::On_Button_Clicked() {
    ui.progressBar->setValue(50);
    ui.BlockLabel->setText(ui.BlocksizeIn->text());
    ui.treeWidget->currentItem()->setText(0,"worked");
}