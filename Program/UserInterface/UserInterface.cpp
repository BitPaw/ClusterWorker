#include "UserInterface.h"
#include <wchar.h>
#include <File/OpenFileDialog.h>

UserInterface::UserInterface(QWidget *parent) : QMainWindow(parent)
{
    ui.setupUi(this);

    _server.EventCallBackSocket = this;
    _server.EventCallBackServer = this;

    ui.TextBoxPort->setText("25666");
    ui.TableClientList->setColumnWidth(0, 10);

    connect(ui.ButtonOpenServer, &QPushButton::clicked, this, &UserInterface::OnButtonOpenServerClicked);

    connect(ui.ButtonClientFileSelect, &QPushButton::clicked, this, &UserInterface::OnButtonClientFileSelectClicked);
    connect(ui.ButtonServerFileSelect, &QPushButton::clicked, this, &UserInterface::OnButtonServerFileSelectClicked);
    connect(ui.ButtonDeployApplication, &QPushButton::clicked, this, &UserInterface::OnButtonDeployApplicationClicked);

    connect(ui.ButtonWorkFileSelect, &QPushButton::clicked, this, &UserInterface::OnButtonWorkFileSelectClicked);
}

void UserInterface::OnButtonOpenServerClicked()
{
    QString& portText = ui.TextBoxPort->text();
    unsigned short port = portText.toInt();

    _server.Start(port);
}

void UserInterface::OnButtonDeployApplicationClicked()
{
    QString& filePathQ = ui.TextBoxClientFile->text();
    bool hasPath = !filePathQ.isEmpty();

    if (hasPath)
    {
        bool isPathToExistingFile = true; // TODO: Add file::Exists()

        if (isPathToExistingFile)
        {
            wchar_t filePathW[_MAX_FNAME];
            char filePath[_MAX_FNAME];
            char fileName[_MAX_FNAME];
            char exten[_MAX_EXT];
            char message[270];

            memset(filePathW, 0, _MAX_FNAME * sizeof(wchar_t));
            memset(message, 0, 270 * sizeof(char));

            size_t sizeOfWchar = filePathQ.toWCharArray(filePathW);
            sprintf(filePath, "%ls", filePathW);

            {
                char drive[_MAX_DRIVE];
                char dir[_MAX_DIR];             

                _splitpath(filePath,drive, dir, fileName, exten);
            }        

            size_t bytesToSend = sprintf(message, "U####%s%s", fileName, exten);

            message[1] = (sizeOfWchar & 0xFF000000) >> 24;
            message[2] = (sizeOfWchar & 0x00FF0000) >> 16;
            message[3] = (sizeOfWchar & 0x0000FF00) >> 8;
            message[4] = sizeOfWchar & 0x000000FF;

            _server.BroadcastMessageToClients(message, bytesToSend + 1 * sizeof(char));      
            _server.BroadcastFileToClients(filePath);
        }      
    }   
}

void UserInterface::OnButtonClientFileSelectClicked()
{
    OpenFileAndSelect(*ui.TextBoxClientFile);
}

void UserInterface::OnButtonWorkFileSelectClicked()
{
    OpenFileAndSelect(*ui.TextBoxWorkFilePath);    
}

void UserInterface::OnButtonServerFileSelectClicked()
{
    OpenFileAndSelect(*ui.TextBoxServerFile);
}

void UserInterface::OnClientDisconnected(BF::Client& client)
{
    // TODO: remove client from table (ClientList)
}

void UserInterface::OnConnectionLinked(int socketID)
{
    char charBuffer[2048];

    sprintf(charBuffer, "[You][%i] Connection Accepted.\n", socketID);

    ui.TextConsole->insertPlainText(charBuffer);    
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

void UserInterface::OnMessageSend(BF::IOSocketMessage socketMessage)
{
    char charBuffer[2048];

    sprintf(charBuffer, "[You][%i] Message (%i Bytes)\n", socketMessage.SocketID, socketMessage.MessageSize);

    ui.TextConsole->insertPlainText(charBuffer);
}

void UserInterface::OnMessageReceive(BF::IOSocketMessage socketMessage)
{
    char charBuffer[2048];

    sprintf(charBuffer, "[Client][%i] Message (%i Bytes)\n", socketMessage.SocketID, socketMessage.MessageSize);

    ui.TextConsole->insertPlainText(charBuffer);
}

void UserInterface::OnClientConnected(BF::Client& client)
{
    QTableWidget& tableWidget = *(ui.TableClientList);

    char textID[255];
    sprintf(textID, "%i", client.ID);

    int rowCount = tableWidget.rowCount();

    tableWidget.insertRow(rowCount);
    tableWidget.setItem(rowCount, 0, new QTableWidgetItem(textID));
    tableWidget.setItem(rowCount, 1, new QTableWidgetItem("Client"));
    tableWidget.setItem(rowCount, 2, new QTableWidgetItem("Conneced"));

    // tableWidget.Width
}

void UserInterface::OpenFileAndSelect(QLineEdit& lineEdit)
{
    char filePath[260];
    bool successful = BF::OpenFileDialog::Open(filePath);

    if (successful)
    {
        lineEdit.setText(filePath);
    }
}