#pragma once

#include <QObject>
#include <QTcpSocket>
#include <QByteArray>
#include "protocol.h"
#include <memory>

class ManagerNetwork : public QObject
{
    Q_OBJECT
public:
    explicit ManagerNetwork(QObject *parent = nullptr);
    ~ManagerNetwork();
    void sendPacket(std::shared_ptr<Packet> packet);
    void connectToServer(const QString &server, qint16 port);
signals:
    void connected();
    void packetReceived(std::shared_ptr<Packet> packet);
    void error(const QString mes);

private slots:
    void onReadyRead();
    void onConnected();
    void onError(QAbstractSocket::SocketError socketError);
private:
    QTcpSocket socket;
    QByteArray buf;
};




