#include "Packetrouter.h"

#include "packethandler.h"

PacketRouter::PacketRouter(QObject *parent)
    : QObject(parent)
{
}

void PacketRouter::registerHandler(PacketHandler *handler)
{
    handlers.append(handler);
}

void PacketRouter::routePacket(const QByteArray& data) {
    std::shared_ptr<Packet> packet = Packet::deserialize(data);

    if (!packet) {
        qDebug() << "Ошибка десериализации пакета";
        return;
    }

    for (PacketHandler* handler : handlers) {
        packet->handle(handler);
    }
}
