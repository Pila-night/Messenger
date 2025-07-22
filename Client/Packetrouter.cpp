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

void PacketRouter::routePacket(const QByteArray &data)
{
    std::shared_ptr<Packet> packet = Packet::deserialize(data);

    if (!packet) {
        qDebug()<<"Ошибка десериализации пакета";
        return;
    }

    for (int i = 0; i < handlers.size(); ++i) {
        PacketHandler* handler = handlers[i];

        switch (packet->getType()) {
        case PacketType::Register: {
            PacketRegister* p = dynamic_cast<PacketRegister*>(packet.get());
            if (p) handler->handle(*p);
            break;
        }
        case PacketType::Message: {
            PacketMessage* p = dynamic_cast<PacketMessage*>(packet.get());
            if (p) handler->handle(*p);
            break;
        }
        case PacketType::ServerResponse: {
            PacketServerResponse* p = dynamic_cast<PacketServerResponse*>(packet.get());
            if (p) handler->handle(*p);
            break;
        }
        case PacketType::ChatList: {
            PacketChatList* p = dynamic_cast<PacketChatList*>(packet.get());
            if (p) handler->handle(*p);
            break;
        }
        default:
            emit unknownPacketType(static_cast<int>(packet->getType()));
            break;
        }
    }
}
