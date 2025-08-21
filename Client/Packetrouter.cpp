#include "PacketRouter.h"
#include "packethandler.h"
#include "logger.h"

/**
 * @brief Конструктор класса PacketRouter.
 * @param parent.
 */
PacketRouter::PacketRouter(QObject* parent)
    : QObject(parent) {}

/**
 * @brief Деструктор класса PacketRouter.
 * Освобождает память, выделенную под обработчики.
 */
PacketRouter::~PacketRouter() {
    Logger& logger = Logger::getInstance();
    for (auto handler : handlers) {
        logger.log(QtInfoMsg, QString("Освобождение памяти для обработчика: %1")
                                     .arg(reinterpret_cast<quintptr>(handler)));
        delete handler;
    }
    handlers.clear();
    logger.log(QtInfoMsg, "Все обработчики удалены.");
}

/**
 * @brief Регистрирует новый обработчик пакетов.
 * @param handler Указатель на обработчик.
 */
void PacketRouter::registerHandler(PacketHandler* handler) {
    Logger& logger = Logger::getInstance();
    if (handler && !handlers.contains(handler)) {
        handlers.append(handler);
        logger.log(QtInfoMsg, QString("Обработчик зарегистрирован: %1").arg(reinterpret_cast<quintptr>(handler)));
    } else {
        logger.log(QtWarningMsg, "Попытка зарегистрировать уже существующий или некорректный обработчик!");
    }
}

/**
 * @brief Маршрутизирует пакет к соответствующему обработчику.
 * @param data Сериализованные данные пакета.
 */
void PacketRouter::routePacket(const QByteArray& data) {
    Logger& logger = Logger::getInstance();

    auto packet = Packet::deserialize(data);
    if (!packet) {
        logger.log(QtWarningMsg, "Не удалось десериализовать пакет!");
        return;
    }

    PacketType type = packet->getType();
    logger.log(QtInfoMsg, QString("Получен пакет типа: %1").arg(static_cast<int>(type)));

    bool handled = false;

    for (auto handler : handlers) {
        switch (type) {
        case PacketType::Message:
            if (dynamic_cast<PacketMessageHandler*>(handler)) {
                packet->handle(handler);
                logger.log(QtInfoMsg, QString("Пакет типа Message обработан обработчиком: %1")
                                          .arg(reinterpret_cast<quintptr>(handler)));
                handled = true;
            }
            break;

        case PacketType::ServerResponse:
            if (dynamic_cast<PacketServerResponseHandler*>(handler)) {
                packet->handle(handler);
                logger.log(QtInfoMsg, QString("Пакет типа ServerResponse обработан обработчиком: %1")
                                            .arg(reinterpret_cast<quintptr>(handler)));
                handled = true;
            }
            break;

        case PacketType::ChatList:
            if (dynamic_cast<PacketChatListHandler*>(handler)) {
                packet->handle(handler);
                logger.log(QtInfoMsg, QString("Пакет типа ChatList обработан обработчиком: %1")
                                             .arg(reinterpret_cast<quintptr>(handler)));
                handled = true;
            }
            break;

        default:
            logger.log(QtWarningMsg, QString("Не найден подходящий обработчик для типа пакета: %1")
                                               .arg(static_cast<int>(type)));
            break;
        }
    }

    if (!handled) {
        logger.log(QtWarningMsg, "Пакет не был обработан ни одним из зарегистрированных обработчиков.");
    }
}
