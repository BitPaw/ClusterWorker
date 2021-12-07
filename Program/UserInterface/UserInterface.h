#pragma once

#include <QtWidgets/QMainWindow>
#include <Network/Server.h>
#include <Network/ISocketListener.h>
#include <Network/IServerListener.h>

#include "ui_UserInterface.h"

class UserInterface : public QMainWindow, public BF::ISocketListener, public BF::IServerListener
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

private:
    Ui::UserInterfaceClass ui;

    BF::Server _server;

    void OpenFileAndSelect(QLineEdit& lineEdit);
    void ButtonEnable(QPushButton& button, bool enable);

    void CheckDeployButton();

    //---<Geerbt über ISocketListener>-----------------------------------------
    virtual void OnConnectionLinked(int socketID) override;
    virtual void OnConnectionListening(int socketID) override;
    virtual void OnConnectionEstablished(int socketID) override;
    virtual void OnConnectionTerminated(int socketID) override;
    virtual void OnMessageSend(BF::IOSocketMessage socketMessage) override;
    virtual void OnMessageReceive(BF::IOSocketMessage socketMessage) override;

    // Geerbt über IServerListener
    virtual void OnClientConnected(BF::Client& client) override;
    virtual void OnClientDisconnected(BF::Client& client) override;
    //-------------------------------------------------------------------------
};
