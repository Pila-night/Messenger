#ifndef PACKETROUTER_H
#define PACKETROUTER_H
#include <QObject>
#include "protocol.h"
#include "packethandler.h"
#include <QTcpSocket>
#include <QList>

class PacketRouter : public QObject
{
    Q_OBJECT
public:
    PacketRouter(QObject *parent = nullptr);

    void registerHandler(PacketHandler* handler);
    void routePacket(QTcpSocket* socket, const QByteArray& data);

private:
    QVector<PacketHandler*> handlers;
};
#endif // PACKETROUTER_H
