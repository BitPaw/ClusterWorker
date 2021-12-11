#pragma once

#include <QtWidgets/QMainWindow>
#include <Network/Server.h>
#include <Network/ISocketListener.h>
#include <Network/IServerListener.h>
#include <File/ProgramExecuteResultListener.hpp>
#include <ErrorCode.h>

#include "ui_UserInterface.h"
#include "ServerState.h"
#include "UserInteractLevel.h"


class UserInterface :   public QMainWindow, 
                        public BF::ISocketListener, 
                        public BF::IServerListener,
                        public BF::ProgramExecuteResultListener
{
    Q_OBJECT

public:
    UserInterface(QWidget *parent = Q_NULLPTR);
   
public slots:
    void OnButtonOpenServerClicked();

    void OnButtonResultFileSelectClicked();
    void OnButtonClientFileSelectClicked();
    void OnButtonServerFileSelectClicked();
    void OnButtonInputFileSelectClicked();

    void OnButtonDeployApplicationClicked();

    void OnButtonStartClicked();

private:
    Ui::UserInterfaceClass ui;

    BF::Server _server;
    ServerState _stateCurrent;

    void OpenFileAndSelect(QLineEdit& lineEdit);
    void ButtonEnable(QPushButton& button, UserInteractLevel userInteractLevel);

    UserInteractLevel CanUserPressDeployButton();
    UserInteractLevel CanUserAbort();
    UserInteractLevel CanUserStart();
    UserInteractLevel CanUserOpenServer();

    void CheckDeployButton();
    void CheckStartButton();
    void CheckAbortButton();
    void CheckOpenServerButton();

    void WriteToConsole(char* text);

    void StateChange(ServerState state);

    void TextBoxToCharArray(QLineEdit& textbox, char* buffer);

    //---<Geerbt über ISocketListener>-----------------------------------------
    void OnConnectionLinked(const BF::IPAdressInfo& adressInfo);
    void OnConnectionListening(const BF::IPAdressInfo& adressInfo);
    void OnConnectionEstablished(const BF::IPAdressInfo& adressInfo);
    void OnConnectionTerminated(const BF::IPAdressInfo& adressInfo);
    void OnMessageSend(BF::IOSocketMessage socketMessage);
    void OnMessageReceive(BF::IOSocketMessage socketMessage);
    void OnClientConnected(BF::Client& client);
    void OnClientDisconnected(BF::Client& client);
    void OnSocketCreating(const BF::IPAdressInfo& adressInfo, bool& use);
    void OnSocketCreated(const BF::IPAdressInfo& adressInfo, bool& use);
    void OnClientAcceptFailure();
    void OnProgramExecuted(bool succesful, size_t returnResult, BF::ErrorCode errorCode) override;
    //-------------------------------------------------------------------------
};
