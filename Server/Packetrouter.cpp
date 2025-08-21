#include "Packetrouter.h"
#include "packethandler.h"
#include "logger.h"
PacketRouter::PacketRouter(QObject *parent)
    : QObject(parent)
{
}

void PacketRouter::registerHandler(PacketHandler* handler) {
    Logger& logger = Logger::getInstance();
    if (handler && !handlers.contains(handler)) {
        handlers.append(handler);
        logger.log(QtInfoMsg, QString("Обработчик зарегистрирован: %1").arg(reinterpret_cast<quintptr>(handler)));
    } else {
        logger.log(QtWarningMsg, "Попытка зарегистрировать уже существующий или некорректный обработчик!");
    }
}

void PacketRouter::routePacket(QTcpSocket* socket, const QByteArray& data) {
    std::shared_ptr<Packet> packet = Packet::deserialize(data);
    Logger& logger = Logger::getInstance();
    if (!packet) {
        logger.log(QtWarningMsg, "Не удалось десериализовать пакет!");
        return;
    }


    PacketType type = packet->getType();
    logger.log(QtInfoMsg, QString("Получен пакет типа: %1").arg(static_cast<int>(type)));
    bool handled = false;
    for (PacketHandler* handler : handlers) {
        switch (type) {
        case PacketType::Register: {
            PacketRegister* registerPacket = dynamic_cast<PacketRegister*>(packet.get());
            if (registerPacket != nullptr) {
                handler->handle(socket, *registerPacket);
                handled = true;
            }
            break;
        }
        case PacketType::Auth: {
            PacketAuth* packetAuth = dynamic_cast<PacketAuth*>(packet.get());
            if (packetAuth != nullptr) {
                handler->handle(socket, *packetAuth);
                handled = true;
            }
            break;
        }
        case PacketType::Message: {
            PacketMessage* messagePacket = dynamic_cast<PacketMessage*>(packet.get());
            if (messagePacket) {
                handler->handle(socket, *messagePacket);
                handled = true;
            }
            break;
        }
        case PacketType::ServerResponse: {
            PacketServerResponse* serverResponsePacket = dynamic_cast<PacketServerResponse*>(packet.get());
            if (serverResponsePacket) {
                handler->handle(socket, *serverResponsePacket);
                handled = true;
            }
            break;
        }
        case PacketType::ChatList: {
            PacketChatList* packetChatList = dynamic_cast<PacketChatList*>(packet.get());
            if (packetChatList) {
                handler->handle(socket, *packetChatList);
                handled = true;
            }
            break;
        }
        default:
            break;
        }
        if (!handled) {
            logger.log(QtWarningMsg, QString("Пакет типа %1 не был обработан ни одним из зарегистрированных обработчиков.")
                                         .arg(static_cast<int>(type)));
    }
}
}
