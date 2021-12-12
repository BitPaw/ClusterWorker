#include "UserInterface.h"
#include <wchar.h>
#include <File/OpenFileDialog.h>
#include <File/Program.h>
#include <File/File.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "DeployWorkInfo.h"

#define ColorRed "FF4444"
#define ColorGreen "44FF44"
#define ColorYellow "FFFF44"
#define DefaultPort "25666"

UserInterface::UserInterface(QWidget *parent) : QMainWindow(parent)
{
    ui.setupUi(this);

    _server.EventCallBackSocket = this;
    _server.EventCallBackServer = this;


    _clientInfoListSizeCurrent = 0;
    _clientInfoListSizeMaximal = 10;
    _clientInfoList = new ClientInfo[_clientInfoListSizeMaximal];


    _textConsoleAsyncLock.Create();

    ui.TextBoxPort->setText(DefaultPort);
    //ui.TableClientList->setColumnWidth(0, 10);

    ui.ProgressBarWork->setValue(0);
    ui.ProgressBarDeployApplication->setValue(0);

    ui.ComboBoxIPMode->addItem("Both");
    ui.ComboBoxIPMode->addItem("IPv4");
    ui.ComboBoxIPMode->addItem("IPv6");

    connect(ui.ButtonOpenServer, &QPushButton::clicked, this, &UserInterface::OnButtonOpenServerClicked);

    connect(ui.ButtonInputFileSelect, &QPushButton::clicked, this, &UserInterface::OnButtonInputFileSelectClicked);
    connect(ui.ButtonClientFileSelect, &QPushButton::clicked, this, &UserInterface::OnButtonClientFileSelectClicked);
    connect(ui.ButtonServerFileSelect, &QPushButton::clicked, this, &UserInterface::OnButtonServerFileSelectClicked);
    connect(ui.ButtonResultFileSelect, &QPushButton::clicked, this, &UserInterface::OnButtonResultFileSelectClicked);

    connect(ui.ButtonDeployApplication, &QPushButton::clicked, this, &UserInterface::OnButtonDeployApplicationClicked);
    connect(ui.ButtonStart, &QPushButton::clicked, this, &UserInterface::OnButtonStartClicked);

    connect(ui.TextBoxPort, &QLineEdit::textChanged, this, &UserInterface::CheckOpenServerButton);

    ui.TextConsole->ensureCursorVisible();

    StateChange(ServerState::IDLE); // Check all buttons
}

void UserInterface::OnButtonOpenServerClicked()
{
    UserInteractLevel userInteractLevel = CanUserOpenServer();

    switch (userInteractLevel)
    {
        case UserInteractLevel::NoInteraction:
        {
            int msgboxID = MessageBox
            (
                NULL,
                L"The server might be already running\n"
                "In any case, you can't open the server now.",
                (LPCWSTR)L"Can't open server",
                MB_ICONWARNING | MB_OK | MB_TOPMOST
            );

            return;
        }

        case UserInteractLevel::LimitedInteraction:
        {
            int msgboxID = MessageBox
            (
                NULL,
                L"The entered port might be faulty.\n"
                "Valid ports are 1-4000.\n\n",
                (LPCWSTR)L"Invalid port",
                MB_ICONWARNING | MB_OK | MB_TOPMOST
            );

            return;
        }
    }

    StateChange(ServerState::ServerOpening);

    QString& portText = ui.TextBoxPort->text();
    unsigned short port = portText.toInt();

    BF::SocketActionResult result = _server.Start(port);

    /*
    if (result == BF::SocketActionResult::Successful)
    {
        ui.ButtonOpenServer->setDisabled(true);        
    }*/

    StateChange(ServerState::ServerOpened);
}

void UserInterface::OnButtonDeployApplicationClicked()
{
    UserInteractLevel canPressButton = CanUserPressDeployButton();

    switch (canPressButton)
    {
        case UserInteractLevel::NoInteraction:
        {
            int msgboxID = MessageBox
            (
                NULL,
                L"You can't deploy data right now.\n"
                "Following solutions may help:\n\n"
                "A) The server is not open.\n"
                "B) You already deployed\n",
                (LPCWSTR)L"Can't deploy data",
                MB_ICONWARNING | MB_OK | MB_TOPMOST
            );

            return;
        }
        case UserInteractLevel::LimitedInteraction:
        {
            int msgboxID = MessageBox
            (
                NULL,
                L"You need to specify all four paths.\n"
                "Without them, the system can not perform.\n\n"
                "Please select all paths and try again.",
                (LPCWSTR)L"Paths missing",
                MB_ICONWARNING | MB_OK | MB_TOPMOST
            );

            return;
        }
    }

    ui.ProgressBarDeployApplication->setValue(10);

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

            size_t bytesToSend = sprintf(message, "E####%s%s", fileName, exten);

            message[1] = (sizeOfWchar & 0xFF000000) >> 24;
            message[2] = (sizeOfWchar & 0x00FF0000) >> 16;
            message[3] = (sizeOfWchar & 0x0000FF00) >> 8;
            message[4] = sizeOfWchar & 0x000000FF;

            ui.ProgressBarDeployApplication->setValue(20);

            StateChange(ServerState::DataDeploying);

            _server.BroadcastMessageToClients(message, bytesToSend + 1 * sizeof(char));   

            ui.ProgressBarDeployApplication->setValue(50);

            _server.BroadcastFileToClients(filePath);

            ui.ProgressBarDeployApplication->setValue(100);

            StateChange(ServerState::DataDeployed);
        }      
    }   
}

void UserInterface::OnButtonStartClicked()
{
    UserInteractLevel canPressButton = CanUserStart();

    switch (canPressButton)
    {
        case UserInteractLevel::NoInteraction:
        {
            int msgboxID = MessageBox
            (
                NULL,
                L"You can't start right now.\n"
                "Following things are needed:\n\n"
                "1) Server needs to be open.\n"
                "B) You need to have data deployed.\n",
                (LPCWSTR)L"Can't start system",
                MB_ICONWARNING | MB_OK | MB_TOPMOST
            );

            return;
        }
        case UserInteractLevel::LimitedInteraction:
        {
           // No usecase yet
            return;
        }
    }

    StateChange(ServerState::Starting);

    ui.ProgressBarWork->setValue(10);

    char serverExecutableFilePath[260];
    char serverExecutableInputFilePath[260];

    TextBoxToCharArray(*ui.TextBoxServerWorkFilePath, serverExecutableInputFilePath);
    TextBoxToCharArray(*ui.TextBoxServerFile, serverExecutableFilePath);

    ui.ProgressBarWork->setValue(25);

    //BF::Program::Execute(serverExecutableFilePath, serverExecutableInputFilePath, this);

    ui.ProgressBarWork->setValue(50);

    StateChange(ServerState::Started);
}

void UserInterface::OnButtonResultFileSelectClicked()
{
    OpenFileAndSelect(*ui.TextBoxResultFile);

    CheckDeployButton();
}

void UserInterface::OnButtonClientFileSelectClicked()
{
    OpenFileAndSelect(*ui.TextBoxClientFile);

    CheckDeployButton();
}

void UserInterface::OnButtonInputFileSelectClicked()
{
    OpenFileAndSelect(*ui.TextBoxServerWorkFilePath);    

    CheckDeployButton();
}

void UserInterface::OnButtonServerFileSelectClicked()
{
    OpenFileAndSelect(*ui.TextBoxServerFile);

    CheckDeployButton();
}

void UserInterface::OnClientDisconnected(BF::Client& client)
{
    // TODO: remove client from table (ClientList)
}

void UserInterface::OnSocketCreating(const BF::IPAdressInfo& adressInfo, bool& use)
{
}

void UserInterface::OnSocketCreated(const BF::IPAdressInfo& adressInfo, bool& use)
{

}

void UserInterface::OnClientAcceptFailure()
{

}

void UserInterface::OnProgramExecuted(bool succesful, intptr_t returnResult, BF::ErrorCode errorCode)
{
    char charBuffer[2048];

    sprintf(charBuffer, "[Server] Program executed succesfully <R:%zi E:%i>.\n", returnResult, errorCode);

    WriteToConsole(charBuffer);

    ui.ProgressBarWork->setValue(50);

    if (!succesful)
    {
        wchar_t msgBoxText[1024];

        swprintf
        (
            msgBoxText, 
            L"The server-side application was executed sucessfuly but returned an error\n\n"
            "Details: ReturnCode - %zi\n"
            "         ErrorCode  - %zi\n",
            returnResult,
            errorCode
         );

        int msgboxID = MessageBox
        (
            NULL,
            msgBoxText,
            (LPCWSTR)L"Execution failed",
            MB_ICONERROR | MB_OK | MB_TOPMOST
        );

        ui.ProgressBarWork->setValue(0);

        return;
    }

    DeployWorkInfo* deployWorkInfo = new DeployWorkInfo();
    deployWorkInfo->UI = this;
    deployWorkInfo->ServerSystem = &_server;
    strncpy_s(deployWorkInfo->folderPath, "/*", 260);

    _workDeployer.Run(UserInterface::DeployWorkTasksAsync, deployWorkInfo);
}

ThreadFunctionReturnType UserInterface::DeployWorkTasksAsync(void* data)
{
    DeployWorkInfo* deployWorkInfo = (DeployWorkInfo*)data;
    char* folderPath = deployWorkInfo->folderPath;
    BF::Server* server = deployWorkInfo->ServerSystem;
    UserInterface* ui = deployWorkInfo->UI;
    wchar_t** fileList = nullptr;
    size_t fileListSize = 0;

    BF::File::FilesInFolder(folderPath, &fileList, fileListSize);

    for (size_t currentFileNumber = 0; currentFileNumber < fileListSize; )
    {
        ClientInfo* clientInfo = ui->GetNextFreeClient();
        wchar_t* filePath = fileList[currentFileNumber];

        if (!clientInfo)
        {
            Sleep(1000); // If there is no client, wait 1s and try again.
            continue;
        }  

        char filePathA[512];

        wcstombs(filePathA, filePath, 512);

        char message[260];

        sprintf(message, "W\0\0\0\0\0");

        BF::SocketActionResult sendMResult = server->SendMessageToClient(clientInfo->SocketID, filePathA, 5);

        if (sendMResult == BF::SocketActionResult::Successful)
        {
            BF::SocketActionResult sendFResult = server->SendFileToClient(clientInfo->SocketID, filePathA);

            if (sendFResult == BF::SocketActionResult::Successful)
            {
                clientInfo->State = ClientState::Working;
                ++currentFileNumber;
            }
        }
    }

    delete deployWorkInfo;

    return 0;
}

void UserInterface::OnConnectionLinked(const BF::IPAdressInfo& adressInfo)
{
    char charBuffer[2048];

    sprintf(charBuffer, "[You][%i] Connection Accepted.\n", adressInfo.SocketID);

    WriteToConsole(charBuffer);
}

void UserInterface::OnConnectionListening(const BF::IPAdressInfo& adressInfo)
{
    char charBuffer[2048];

    sprintf(charBuffer, "[You][%i] Listen on IP:<%s> Port:<%i>.\n", adressInfo.SocketID, adressInfo.IP, adressInfo.Port);

    WriteToConsole(charBuffer);
}

void UserInterface::OnConnectionEstablished(const BF::IPAdressInfo& adressInfo)
{
    // When Client connect to Server
    // Does not happen here
}

void UserInterface::OnConnectionTerminated(const BF::IPAdressInfo& adressInfo)
{
    char charBuffer[2048];

    sprintf(charBuffer, "[Client][%i] Connection Terminated!\n", adressInfo.SocketID);

    WriteToConsole(charBuffer);
}

void UserInterface::OnMessageSend(BF::IOSocketMessage socketMessage)
{
    char charBuffer[2048];

    sprintf(charBuffer, "[You][%i] Message (%zi Bytes)\n", socketMessage.SocketID, socketMessage.MessageSize);

    WriteToConsole(charBuffer);
}

void UserInterface::OnMessageReceive(BF::IOSocketMessage socketMessage)
{
    char charBuffer[2048];

    sprintf(charBuffer, "[Client][%i] Message (%zi Bytes)\n", socketMessage.SocketID, socketMessage.MessageSize);

    WriteToConsole(charBuffer);
}

void UserInterface::OnClientConnected(BF::Client& client)
{
    QTableWidget& tableWidget = *(ui.TableClientList);

    char* clientName = client.AdressInfo.IP == '\0' ? "Client" : client.AdressInfo.IP;

    char textID[255];
    sprintf(textID, "%i", client.AdressInfo.SocketID);

    int rowCount = tableWidget.rowCount();

    tableWidget.insertRow(rowCount);
    tableWidget.setItem(rowCount, 0, new QTableWidgetItem(textID));
    tableWidget.setItem(rowCount, 1, new QTableWidgetItem(clientName));
    tableWidget.setItem(rowCount, 2, new QTableWidgetItem("Conneced"));
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

void UserInterface::ButtonEnable(QPushButton& button, UserInteractLevel userInteractLevel)
{
    QStyle* style = button.style();
    char buffer[1024];
    char* colorTag = nullptr;

    switch (userInteractLevel)
    {
        case UserInteractLevel::NoInteraction:
            colorTag = ColorRed;
            break;

        case UserInteractLevel::LimitedInteraction:
            colorTag = ColorYellow;
            break;

        case UserInteractLevel::FullInteraction:
            colorTag = ColorGreen;
            break;
    }

    
   // char* color = enable ? "555555" : "CDCDCD";

  //  sprintf(buffer, "color: white;\nBackGround-Color: #%s;", color);

    sprintf(buffer, "color: #%s;\nBackGround-Color: #555555;", colorTag);

    button.setStyleSheet(buffer);
}

UserInteractLevel UserInterface::CanUserPressDeployButton()
{
    bool a = !ui.TextBoxResultFile->text().isEmpty();
    bool b = !ui.TextBoxServerFile->text().isEmpty();
    bool c = !ui.TextBoxServerWorkFilePath->text().isEmpty();
    bool d = !ui.TextBoxClientFile->text().isEmpty();

    bool enable = a && b && c && d;
      
    if (_stateCurrent == ServerState::ServerOpened)
    {
        if (enable)
        {
           return UserInteractLevel::FullInteraction;
        }

        return UserInteractLevel::LimitedInteraction;
    }


    return UserInteractLevel::NoInteraction;
}

UserInteractLevel UserInterface::CanUserAbort()
{
    if (_stateCurrent == ServerState::Started)
    {
        return UserInteractLevel::FullInteraction;
    }

    return UserInteractLevel::NoInteraction;
}

UserInteractLevel UserInterface::CanUserStart()
{
    if (_stateCurrent == ServerState::DataDeployed)
    {
        return UserInteractLevel::FullInteraction;
    }

    return UserInteractLevel::NoInteraction;
}

UserInteractLevel UserInterface::CanUserOpenServer()
{
    if (_stateCurrent == ServerState::IDLE)
    {
        int port = ui.TextBoxPort->text().toInt();
        bool isValidPort = 0 < port && port <= 40000;

        if (isValidPort)
        {
            return UserInteractLevel::FullInteraction;
        }

        return UserInteractLevel::LimitedInteraction;
    }

    return UserInteractLevel::NoInteraction;
}

void UserInterface::CheckDeployButton()
{
    UserInteractLevel canPressButton = CanUserPressDeployButton();

    ButtonEnable(*ui.ButtonDeployApplication, canPressButton);
}

void UserInterface::CheckStartButton()
{
    UserInteractLevel canUserStart = CanUserStart();

    ButtonEnable(*ui.ButtonStart, canUserStart);
}

void UserInterface::CheckAbortButton()
{
    UserInteractLevel canUserAbort = CanUserAbort();

    ButtonEnable(*ui.ButtonAbort, canUserAbort);
}

void UserInterface::CheckOpenServerButton()
{
    UserInteractLevel canUserOpenServer = CanUserOpenServer();

    ButtonEnable(*ui.ButtonOpenServer, canUserOpenServer);
}

void UserInterface::WriteToConsole(char* text)
{
    QTextBrowser* console = ui.TextConsole;

    _textConsoleAsyncLock.Lock();
    console->insertPlainText(text);
    console->moveCursor(QTextCursor::End);
    _textConsoleAsyncLock.Release();
}

void UserInterface::StateChange(ServerState state)
{
    _stateCurrent = state;

    CheckOpenServerButton();
    CheckDeployButton();
    CheckStartButton();
    CheckAbortButton();
}

void UserInterface::TextBoxToCharArray(QLineEdit& textbox, char* buffer)
{
    std::string stdString = textbox.text().toStdString();

    const char* carray = stdString.c_str();

    memset(buffer, 0, 260);
    strncpy_s(buffer, 260, carray, stdString.size());
}

ClientInfo* UserInterface::GetNextFreeClient()
{
    for (size_t i = 0; i < _clientInfoListSizeCurrent; i++)
    {
        ClientInfo& clientInfo = _clientInfoList[i];

        if (clientInfo.State == ClientState::IDLE)
        {
            return &_clientInfoList[i];
        }
    }

    return nullptr;
}

ClientInfo* UserInterface::GetClientViaID(int socketID)
{
    for (size_t i = 0; i < _clientInfoListSizeCurrent; i++)
    {
        ClientInfo& clientInfo = _clientInfoList[i];

        if (clientInfo.SocketID == socketID)
        {
            return &_clientInfoList[i];
        }
    }

    return nullptr;
}