#include "remote_command_listener.h"
#include <iostream>

RemoteCommandListener::RemoteCommandListener(QObject* parent)
    : QObject(parent), tcpServer(new QTcpServer(this)) {
    connect(tcpServer,SIGNAL(newConnection()),this,SLOT(handleConnection()));
}

RemoteCommandListener::~RemoteCommandListener() {
    tcpServer->close();
}

bool RemoteCommandListener::start(uint _portNumber) {
    if (!tcpServer->listen(QHostAddress::Any,_portNumber)) {
        return false;
    }
    std::cout << "server started at port: 3000" << std::endl;
    return true;
}

void RemoteCommandListener::handleConnection() {
    QTcpSocket* clientConnection = tcpServer->nextPendingConnection();
    connect(clientConnection, &QAbstractSocket::disconnected, clientConnection, &QObject::deleteLater);
    connect(clientConnection, &QAbstractSocket::readyRead, this, &RemoteCommandListener::readCommand);
}

void RemoteCommandListener::readCommand() {
    QTcpSocket* clientConnection = static_cast<QTcpSocket*>(sender());
    QJsonDocument d = QJsonDocument::fromJson(clientConnection->readAll());
    QString command = d.object().value("command").toString();
    QString commandData = d.object().value("command_data").toString();

    qDebug() << "received command: " << command;
    qDebug() << "received command data: " << commandData;
    emit commandReceived(ClientCommand(command,commandData));

    clientConnection->write("success\r\n");
    clientConnection->waitForBytesWritten();
    clientConnection->close();
    clientConnection->disconnectFromHost();
}
