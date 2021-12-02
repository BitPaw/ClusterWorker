#include "UserInterface.h"
#include <wchar.h>

UserInterface::UserInterface(QWidget *parent) : QMainWindow(parent)
{
    ui.setupUi(this);

    _server.Callback = this;
    ui.TextBoxPort->setText("25666");
    ui.TableClientList->setColumnWidth(0, 10);

    //connect(ui.butt, &QPushButton::clicked, this, &UserInterface::On_Button_Clicked);

    connect(ui.ButtonOpenServer, &QPushButton::clicked, this, &UserInterface::OnButtonOpenServerClicked);
}

void UserInterface::OnButtonOpenServerClicked()
{
    QString& portText = ui.TextBoxPort->text();
    unsigned short port = portText.toInt();

    _server.Start(BF::IPVersion::IPVersion4, port);
}

void UserInterface::OnConnectionLinked(int socketID)
{
    char charBuffer[2048];

    sprintf(charBuffer, "[You][%i] Connection Accepted.\n", socketID);

    ui.TextConsole->insertPlainText(charBuffer);
    
    
    QTableWidget& tableWidget = *(ui.TableClientList);

    char textID[255];
    sprintf(textID, "%i", socketID);

    int rowCount = tableWidget.rowCount();

    tableWidget.insertRow(rowCount);
    tableWidget.setItem(rowCount, 0, new QTableWidgetItem(textID));
    tableWidget.setItem(rowCount, 1, new QTableWidgetItem("Client"));
    tableWidget.setItem(rowCount, 2, new QTableWidgetItem("Conneced"));

   // tableWidget.Width
}

void UserInterface::OnConnectionListening(int socketID)
{
    char charBuffer[2048];

    sprintf(charBuffer, "[You][%i] Connection Listening...\n", socketID);

    ui.TextConsole->insertPlainText(charBuffer);
}

void UserInterface::OnConnectionEstablished(int socketID)
{
    // When Client connect to Server
    // Does not happen here
}

void UserInterface::OnConnectionTerminated(int socketID)
{
    char charBuffer[2048];

    sprintf(charBuffer, "[Client][%i] Connection Terminated!\n", socketID);

    ui.TextConsole->insertPlainText(charBuffer);
}

void UserInterface::OnMessageSend(int socketID, const char* message, size_t messageSize)
{
    char charBuffer[2048];

    sprintf(charBuffer, "[You][%i] Message:%s (%i Bytes)\n", socketID, message, messageSize);

    ui.TextConsole->insertPlainText(charBuffer);
}

void UserInterface::OnMessageReceive(int socketID, const char* message, size_t messageSize)
{
    char charBuffer[2048];   

    sprintf(charBuffer, "[Client][%i] Message:%s (%i Bytes)\n", socketID, message, messageSize);

    ui.TextConsole->insertPlainText(charBuffer);
}