#pragma once

#include <QtWidgets/QMainWindow>
#include <Network/Server.h>
#include <Network/ISocketListener.h>

#include "ui_UserInterface.h"

class UserInterface : public QMainWindow, public BF::ISocketListener
{
    Q_OBJECT

public:
    UserInterface(QWidget *parent = Q_NULLPTR);
   
public slots:
    void OnButtonOpenServerClicked();
private:
    Ui::UserInterfaceClass ui;

    BF::Server _server;

    //---<Geerbt über ISocketListener>-----------------------------------------
    virtual void OnConnectionLinked(int socketID) override;
    virtual void OnConnectionListening(int socketID) override;
    virtual void OnConnectionEstablished(int socketID) override;
    virtual void OnConnectionTerminated(int socketID) override;
    virtual void OnMessageSend(int socketID, const char* message, size_t messageSize) override;
    virtual void OnMessageReceive(int socketID, const char* message, size_t messageSize) override;
    //-------------------------------------------------------------------------
};
