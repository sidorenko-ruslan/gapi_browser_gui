#ifndef REMOTE_COMMAND_LISTENER_H
#define REMOTE_COMMAND_LISTENER_H
#include <QtNetwork/QTcpServer>
#include "global.h"
#include <QObject>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>

class RemoteCommandListener final : public QObject {
    Q_OBJECT
public:
    explicit RemoteCommandListener(QObject* parent = nullptr);
    virtual ~RemoteCommandListener();
    bool start(uint _portNumber);
signals:
    void commandReceived(const ClientCommand& command);
private slots:
    void handleConnection();
    void readCommand();
    void sendReply(const QString& replyData);

private:
    QTcpServer* tcpServer;
    QTcpSocket* clientConnection;
};

#endif // REMOTE_COMMAND_LISTENER_H
