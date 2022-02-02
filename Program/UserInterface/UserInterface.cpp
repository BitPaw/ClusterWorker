#include "UserInterface.h"

#include <wchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>

#include <File/File.h>
#include <File/OpenFileDialog.h>
#include <File/Program.h>

#include "DeployWorkInfo.h"
#include "UserSafeFile.h"

#define ColorRed "FF4444"
#define ColorGreen "44FF44"
#define ColorYellow "FFFF44"
#define DefaultPort "25666"

#define ServerWorkingFolder "WorkFolder_Server"

UserInterface::UserInterface(QWidget *parent) : QMainWindow(parent)
{
    ui.setupUi(this);

    _server.EventCallBackSocket = this;
    _server.EventCallBackServer = this;

    _tasksDone = 0;
    _tasksToDo = 0;

    _textConsoleAsyncLock.Create();


    SafeFileLoad();


    //---<Setup UI>------------------------------------------------------------
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
    //-------------------------------------------------------------------------

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

    QString portText = ui.TextBoxPort->text();
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

    SafeFileSafe();

    ui.ProgressBarDeployApplication->setValue(10);

    QString filePathQ = ui.TextBoxClientFile->text();
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

            size_t bytesToSend = sprintf(message, "E%s%s", fileName, exten);

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

void UserInterface::GenerateServerExecutableParameters(char* buffer, const int mode, const char* in)
{
    char serverServerWorkingFolderPath[260];
    char serverResultFilePath[260];

    TextBoxToCharArray(*ui.TextBoxResultFile, serverResultFilePath);

    GetServerWorkPath(serverServerWorkingFolderPath);

    sprintf
    (
        buffer,
        "%i \"%s\" \"%s\" \"%s\"",
        mode, // Mode
        in, // Input
        serverResultFilePath, // Out
        serverServerWorkingFolderPath // WorkDir
    );
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

    _elapsedTime.Start();

    ui.ProgressBarWork->setValue(10);

    char serverExecutableFilePath[260];
    char serverExecutableInputFilePath[260];
    char parameterList[1024];

    TextBoxToCharArray(*ui.TextBoxServerFile, serverExecutableFilePath);
    TextBoxToCharArray(*ui.TextBoxServerWorkFilePath, serverExecutableInputFilePath);  

    ui.ProgressBarWork->setValue(25);

    GenerateServerExecutableParameters(parameterList, 0, serverExecutableInputFilePath);

    {
        char consoleOutPut[1024];
        sprintf(consoleOutPut, "[System][#] Starting Executable with parameters.\n%s\n", parameterList);
        WriteToConsole(consoleOutPut);
    }   

    BF::Program::Execute(serverExecutableFilePath, parameterList, this);

    ui.ProgressBarWork->setValue(50);

    StateChange(ServerState::Started);
}

void UserInterface::GetServerWorkPath(char* workPath)
{
    BF::File::WorkingDirectoryGet(workPath, 260);

    size_t length = strlen(workPath);

    strcpy(workPath + length, "/");
    strcpy(workPath + length + 1, ServerWorkingFolder);

    BF::File::DirectoryCreate(workPath);
}

void UserInterface::OnTaskFinished(int clientSocketID)
{
    char consoleOutPut[128];
    char taskCompleted[128];
    char currentTimeText[128];   
    int progressValue = 50.0f + ((++_tasksDone / (float)_tasksToDo) * 50.0f);
    float currentTime = _elapsedTime.TimeStamp();
    bool finished = _tasksDone == _tasksToDo;  

    sprintf(consoleOutPut, "[Client][%i] Finished Task (%zu/%zu). Client is now IDLE\n", clientSocketID, _tasksDone, _tasksToDo);

    WriteToConsole(consoleOutPut);
    
    sprintf(taskCompleted, "%zu/%zu", _tasksDone, _tasksToDo);
    //sprintf(taskCompleted, "%i/%i", _tasksDone, _tasksToDo); 

    //ui.ProgressBarWork->setValue(progressValue);
    //ui.LabelTaskCompletetValue->setText(taskCompleted);  

    if (finished)
    {
        OnAllTasksCompleted();
    }
}

void UserInterface::OnAllTasksCompleted()
{
    _elapsedTime.Stop();
    WriteToConsole("[Server] --- TASK COMPLETED ---\n");
    WriteToConsole("[Server] Combine results...\n");

    wchar_t** fileList = nullptr;
    size_t fileListSize = 0;
    char filePath[260];
    char searchPattern[270];
    char fileName[260];

    GetServerWorkPath(filePath);

    sprintf(fileName, "%s/%s", filePath, "TaskResultFiles.info");
    sprintf(searchPattern, "%s/*.result", filePath);

    BF::File::FilesInFolder(searchPattern, &fileList, fileListSize);

    BF::File::Remove(fileName);
    FILE* file = fopen(fileName, "wb");

    if (!file)
    {
        return; // Error
    }

    for (size_t i = 0; i < fileListSize; i++)
    {
        fprintf(file, "%ls\n", fileList[i]);
    }

    int result = fclose(file);

    if (result)
    {
        return; // Error?
    }


    char serverExecutableFilePath[260];
    char parameterList[1024];

    TextBoxToCharArray(*ui.TextBoxServerFile, serverExecutableFilePath);

    GenerateServerExecutableParameters(parameterList, 1, fileName);

    {
        char consoleOutPut[1024];
        sprintf(consoleOutPut, "[System][#] Starting Executable with parameters.\n%s\n", parameterList);
        WriteToConsole(consoleOutPut);
    }

    BF::Program::Execute(serverExecutableFilePath, parameterList, this);
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

    sprintf(charBuffer, "[Server] Program executed <R:%zi E:%i>.\n", returnResult, errorCode);

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

    if (!deployedWork)
    {
        WriteToConsole("[Server] Deploying work...\n");

        char currentPath[260];

        GetServerWorkPath(currentPath);

        DeployWorkInfo* deployWorkInfo = new DeployWorkInfo();
        deployWorkInfo->Clients = &_clientList;
        deployWorkInfo->ServerSystem = &_server;
        deployWorkInfo->UI = this;

        sprintf(deployWorkInfo->FolderPath, "%s", currentPath);
        sprintf(deployWorkInfo->FolderPathSearchChunk, "%s/*.chunk", currentPath);

        _workDeployer.Run(UserInterface::DeployWorkTasksAsync, deployWorkInfo);

        deployedWork = true;
    } 
    else
    {
        int msgboxID = MessageBox
        (
            NULL,
            L"Operation finished sucessfully",
            (LPCWSTR)L"Execution complete",
            MB_ICONINFORMATION | MB_OK | MB_TOPMOST
        );
    }
}

ThreadFunctionReturnType UserInterface::DeployWorkTasksAsync(void* data)
{
    DeployWorkInfo* deployWorkInfo = (DeployWorkInfo*)data;
    char* folderPath = deployWorkInfo->FolderPath;
    char* folderPathSeachChunks = deployWorkInfo->FolderPathSearchChunk;
    BF::Server* server = deployWorkInfo->ServerSystem;
    ClientList* clientList = deployWorkInfo->Clients;

    wchar_t** fileList = nullptr;
    size_t fileListSize = 0;

    BF::File::FilesInFolder(folderPathSeachChunks, &fileList, fileListSize);

    deployWorkInfo->UI->SetTaskAmount(fileListSize);

    for (size_t currentFileNumber = 0; currentFileNumber < fileListSize; )
    {
        ClientInfo* clientInfo = clientList->GetNextWaiting();
        wchar_t* filePath = fileList[currentFileNumber];

        if (!clientInfo)
        {
            Sleep(1000); // If there is no client, wait 1s and try again.
            continue;
        }  

        char filePathFullA[512];
        char commandDoWork[260];

        sprintf(filePathFullA, "%s/%ls", folderPath, filePath);

        size_t commandSoWorkSize = sprintf(commandDoWork, "W%ls", filePath);

        BF::SocketActionResult sendMResult = server->SendMessageToClient(clientInfo->SocketID, commandDoWork, commandSoWorkSize);

        if (sendMResult == BF::SocketActionResult::Successful)
        {
            BF::SocketActionResult sendFResult = server->SendFileToClient(clientInfo->SocketID, filePathFullA);

            if (sendFResult == BF::SocketActionResult::Successful)
            {
                sprintf(clientInfo->CurrentFileName, "%ls", filePath);
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

    sprintf(charBuffer, "[Server][%i] Listen on IP:<%s> Port:<%i>.\n", adressInfo.SocketID, adressInfo.IP, adressInfo.Port);

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

    _clientList.UnRegister(adressInfo.SocketID);

    sprintf(charBuffer, "[Client][%i] Disconnected!\n", adressInfo.SocketID);

    WriteToConsole(charBuffer);
}

void UserInterface::OnMessageSend(BF::IOSocketMessage socketMessage)
{
    char charBuffer[2048];

    sprintf(charBuffer, "[Server][%i] Message (%zi Bytes)\n", socketMessage.SocketID, socketMessage.MessageSize);

    WriteToConsole(charBuffer);
}

void UserInterface::OnMessageReceive(BF::IOSocketMessage socketMessage)
{
    char charBuffer[2048];
    ClientInfo* clientInfo = _clientList.GetViaID(socketMessage.SocketID);
    
    sprintf(charBuffer, "[Client][%i] Message (%zi Bytes)\n", socketMessage.SocketID, socketMessage.MessageSize);

    switch (clientInfo->State)
    {
        case ClientState::NotConnected:
        {
            break;
        }
        case ClientState::IDLE:
        {
            if (socketMessage.Message[0] == 'N')
            {
                strcpy(clientInfo->Name, socketMessage.Message + 1);
                char consoleOutPut[128];

                sprintf(consoleOutPut, "[Client][%i] Renamed himself to <%s>\n", clientInfo->SocketID, clientInfo->Name);

                WriteToConsole(consoleOutPut);
            }

            break;
        }
        case ClientState::Working:
        {
            if (socketMessage.Message[0] == 'R')
            {
                char fullWorkPath[260];
                char currentWorkPath[260];
                char* filePathWork = clientInfo->CurrentFileName;       

                GetServerWorkPath(currentWorkPath);

                for (size_t i = 0; i < 260; i++)
                {
                    bool isDot = filePathWork[i] == '.';
                  
                    if (isDot)
                    {
                        strcpy(filePathWork + i + 1, "result");
                        break;
                    }
                }              
                
                sprintf(fullWorkPath, "%s/%s", currentWorkPath, filePathWork);

                clientInfo->CurrentFile = fopen(fullWorkPath, "wb");

                assert(clientInfo->CurrentFile);

                clientInfo->State = ClientState::SendingResult;

                char consoleOutPut[128];

                sprintf(consoleOutPut, "[Client][%i] Finished. Recieving result...\n", clientInfo->SocketID);

                WriteToConsole(consoleOutPut);
            }

            break;
        }
        case ClientState::SendingResult:
        {
            assert(clientInfo->CurrentFile);

            fwrite(socketMessage.Message, sizeof(char), socketMessage.MessageSize-1, clientInfo->CurrentFile);

            if (socketMessage.MessageSize < SocketBufferSize)
            {
                int closeResult = fclose(clientInfo->CurrentFile);

                clientInfo->CurrentFile = nullptr;
                memset(clientInfo->CurrentFileName, 0, sizeof(clientInfo->CurrentFileName));

                clientInfo->State = ClientState::IDLE;

                OnTaskFinished(clientInfo->SocketID);          
            }
        }
        default:
        {
            break;
        }
    }

    WriteToConsole(charBuffer);
}

void UserInterface::OnClientConnected(BF::Client& client)
{
    QTableWidget& tableWidget = *(ui.TableClientList);
    char textID[255];
    sprintf(textID, "%i", client.AdressInfo.SocketID);

    client.AdressInfo.IPFromBinaryFormat();

    ClientInfo* info = _clientList.Register(client.AdressInfo.SocketID);

    int rowCount = tableWidget.rowCount();

    tableWidget.insertRow(rowCount);
    tableWidget.setItem(rowCount, 0, new QTableWidgetItem(textID));
    tableWidget.setItem(rowCount, 1, new QTableWidgetItem("Client"));
    tableWidget.setItem(rowCount, 2, new QTableWidgetItem(ClientStateToString(info->State)));
}

void UserInterface::SetTaskAmount(int amountOfTasks)
{
    _tasksDone = 0;
    _tasksToDo = amountOfTasks;
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
    const char* colorTag = nullptr;

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

void UserInterface::WriteToConsole(const char* text)
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

    sprintf(buffer, "%s", carray);
}

void UserInterface::SafeFileLoad()
{
    UserSafeFile userSafeFile;

    userSafeFile.Load();

    ui.TextBoxServerWorkFilePath->setText(userSafeFile.FilePathInput);
    ui.TextBoxClientFile->setText(userSafeFile.FilePathClient);
    ui.TextBoxServerFile->setText(userSafeFile.FilePathServer);
    ui.TextBoxResultFile->setText(userSafeFile.FilePathOutPut);
}

void UserInterface::SafeFileSafe()
{
    UserSafeFile userSafeFile;

    TextBoxToCharArray(*ui.TextBoxServerWorkFilePath, userSafeFile.FilePathInput);
    TextBoxToCharArray(*ui.TextBoxClientFile, userSafeFile.FilePathClient);
    TextBoxToCharArray(*ui.TextBoxServerFile, userSafeFile.FilePathServer);
    TextBoxToCharArray(*ui.TextBoxResultFile, userSafeFile.FilePathOutPut);

    userSafeFile.Save();
}

void UserInterface::SetFilePath(char* path, const char* filePath)
{
    const char* workPath = ServerWorkingFolder;

    sprintf(path, "%s/%s", workPath, filePath);
}