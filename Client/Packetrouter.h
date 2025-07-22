#ifndef PACKETROUTER_H
#define PACKETROUTER_H
#include <QDebug>
#include <QObject>
#include "protocol.h"
#include "packethandler.h"

class PacketRouter : public QObject
{
    Q_OBJECT
public:
    explicit PacketRouter(QObject *parent = nullptr);

    void registerHandler(PacketHandler* handler);
    void routePacket(const QByteArray& data);

signals:
    void deserializationError(const QString& message); /* а надо ли оно? подумать об этом*/
    void unknownPacketType(int packetType);

private:
    QList<PacketHandler*> handlers;
};
#endif // PACKETROUTER_H
