#pragma once

#include <QObject>
#include <QTcpSocket>
#include <QByteArray>


class ManagerNetwork : public QObject
{
    Q_OBJECT
public:
    explicit ManagerNetwork(QObject *parent = nullptr);
    ~ManagerNetwork();
    void sendPacket(QByteArray data);
    void connectToServer(const QString &server, qint16 port);
signals:
    void connected();
    void dataReceived(const QByteArray &data);
    void error(const QString mes);

private slots:
    void packetRead();
    void onConnected();
    void onError(QAbstractSocket::SocketError socketError);
private:
    QTcpSocket socket;
    QByteArray buf;
};




