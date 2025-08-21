#include "packethandler.h"
#include "logger.h"


PacketServerResponseHandler::PacketServerResponseHandler(QObject* parent)
    : QObject(parent) {}

void PacketServerResponseHandler::handle(PacketAuth& packet) {}

void PacketServerResponseHandler::handle(PacketRegister& packet) {}

void PacketServerResponseHandler::handle(PacketMessage& packet) {}

void PacketServerResponseHandler::handle(PacketChatList& packet) {}

void PacketServerResponseHandler::handle(PacketServerResponse& packet) {
    Logger& logger = Logger::getInstance();
    if (packet.GetResponseType() == PacketServerResponse::ServerResponseType::Auth) {
        switch (packet.GetResponseStatus()) {
        case PacketServerResponse::ServerResponseStatus::Success:
            logger.log(QtInfoMsg, "Авторизация успешна.");
            emit authSuccess();
            break;
        case PacketServerResponse::ServerResponseStatus::SuccessUsername:
            logger.log(QtInfoMsg, "Получена соль от сервера");
            emit authSaltReceived(packet.GetSalt());
            break;
        case PacketServerResponse::ServerResponseStatus::Failed:
            logger.log(QtWarningMsg, QString("Ошибка авторизации: %1")
                                         .arg(packet.getResponse()));
            emit authFailed(packet.getResponse());
            break;
        default:
            logger.log(QtWarningMsg, "Получен неизвестный статус авторизации.");
            emit authFailed("Получен неизвестный статус авторизации");
            break;
        }
    }

    if (packet.GetResponseType() == PacketServerResponse::ServerResponseType::Register) {
        if (packet.getRegisterStatus()) {
            logger.log(QtInfoMsg, "Регистрация успешна.");
            emit RegisterSuccess();
        } else {
            logger.log(QtWarningMsg, QString("Ошибка регистрации: %1").arg(packet.getResponse()));
            emit RegisterFailed(packet.getResponse());
        }
    }
}


PacketMessageHandler::PacketMessageHandler(QObject* parent)
    : QObject(parent) {}

void PacketMessageHandler::handle(PacketAuth& packet) {}

void PacketMessageHandler::handle(PacketRegister& packet) {}

void PacketMessageHandler::handle(PacketServerResponse& packet) {}

void PacketMessageHandler::handle(PacketChatList& packet) {}

void PacketMessageHandler::handle(PacketMessage& packet) {
    QString firstName = packet.getFirstName();
    QString lastName = packet.getLastName();
    QString chatName = packet.getChatName();
    QString sender = packet.getFrom();
    QString text = packet.getText();
    QDateTime timestamp = packet.getTimestamp();
    Logger& logger = Logger::getInstance();
    logger.log(QtInfoMsg, QString("Получено новое сообщение в чате '%1' от %2 %3 (%4): %5")
                              .arg(chatName, firstName, lastName, sender, text));

    emit messageReceived(firstName, lastName, chatName, sender, text, timestamp);
}


PacketChatListHandler::PacketChatListHandler(QObject* parent)
    : QObject(parent) {}

void PacketChatListHandler::handle(PacketAuth& packet) {}

void PacketChatListHandler::handle(PacketRegister& packet) {}

void PacketChatListHandler::handle(PacketMessage& packet) {}

void PacketChatListHandler::handle(PacketServerResponse& packet) {}

void PacketChatListHandler::handle(PacketChatList& packet) {
    QStringList chatList = packet.getChatNames();
    Logger& logger = Logger::getInstance();
    logger.log(QtInfoMsg, "Получен список чатов");
    emit chatListReceived(chatList);
}
