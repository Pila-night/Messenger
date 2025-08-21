#ifndef PACKETROUTER_H
#define PACKETROUTER_H
#include <QObject>
#include "protocol.h"
#include "packethandler.h"

class PacketRouter : public QObject
{
    Q_OBJECT
public:
    PacketRouter(QObject *parent = nullptr);

    void registerHandler(PacketHandler* handler);
    void routePacket(const QByteArray& data);
    ~PacketRouter() override;
private:
    QList<PacketHandler*> handlers;
};
#endif // PACKETROUTER_H
