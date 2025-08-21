#pragma once

#include <QObject>
#include <QTcpSocket>
#include <QByteArray>


class ManagerNetwork : public QObject
{
    Q_OBJECT
public:
    ManagerNetwork(QObject *parent = nullptr);
    ~ManagerNetwork();
    void sendPacket(QByteArray data);
    void connectToServer(const QString &server, qint16 port);
    void disconnectFromServer();
    bool isConnected();
signals:
    void connected();
    void dataReceived(const QByteArray &data);

private slots:
    void packetRead();
    void onConnected();
private:
    QTcpSocket socket;
    QByteArray buf;
};




