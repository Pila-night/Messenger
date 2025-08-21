#include "PacketHandler.h"
#include "SecurityUtils.h"
#include "logger.h"

PacketRegisterHandler::PacketRegisterHandler(ClientDataBase* db, ManagerNetwork* managerNetwork, QObject* parent)
    : QObject(parent), clientDataBase(db), managerNetwork(managerNetwork) {

}

void PacketRegisterHandler::handle(QTcpSocket* socket, PacketRegister& packet) {
    QString username = packet.getUsername();
    QString password = packet.getPassword();
    QString first_name = packet.getFirst_name();
    QString last_name = packet.getLast_name();
    Logger& logger = Logger::getInstance();
    logger.log(QtDebugMsg, QString("Начинаю обработку запроса "
                                   "регистрации для пользователя %1").arg(username));
    bool isUsernameTaken = clientDataBase->existsUser(username);

    if (!isUsernameTaken) {
        salt = SecurityUtils::generateSalt(16);
        QString hash = SecurityUtils::hashPassword(password, salt);

        if (clientDataBase->createUser(first_name, last_name, username, hash, salt)) {
            PacketServerResponse response;
            response.SetResponseType(PacketServerResponse::ServerResponseType::Register);
            response.SetResponseStatus(PacketServerResponse::ServerResponseStatus::Success);
            QByteArray serialized = response.serialize();
            managerNetwork->sendMessageToUser(socket, serialized);
            logger.log(QtInfoMsg, QString("Пользователь %1 успешно зарегистрирован").arg(username));
        } else {
            PacketServerResponse response;
            response.SetResponseType(PacketServerResponse::ServerResponseType::Register);
            response.SetResponseStatus(PacketServerResponse::ServerResponseStatus::Failed);
            response.SetResponseMessage("Ошибка на стороне сервера: не удалось добавить пользователя в БД");
            QByteArray serialized = response.serialize();
            managerNetwork->sendMessageToUser(socket, serialized);
            logger.log(QtWarningMsg, QString("Не удалось зарегистрировать "
                                             "пользователя %1: ошибка базы данных").arg(username));
        }
    } else { // Логин занят
        PacketServerResponse response1;
        response1.SetResponseType(PacketServerResponse::ServerResponseType::Register);
        response1.SetResponseStatus(PacketServerResponse::ServerResponseStatus::Failed);
        response1.SetResponseMessage("Данный логин уже занят");
        QByteArray serialized1 = response1.serialize();
        managerNetwork->sendMessageToUser(socket, serialized1);
        logger.log(QtWarningMsg, QString("Попытка регистрации с занятым логином: %1").arg(username));
    }
}





PacketAuthHandler::PacketAuthHandler(ClientDataBase* db, ManagerNetwork* managerNetwork, QObject* parent)
    : QObject(parent), clientDataBase(db), managerNetwork(managerNetwork) {}

void PacketAuthHandler::handle(QTcpSocket* socket, PacketAuth& packet) {
    QString username = packet.getUsername();
    QString password = packet.getPassword();

    Logger& logger = Logger::getInstance();
    logger.log(QtDebugMsg, QString("Начинаю обработку запроса аутентификации "
                                   "для пользователя %1").arg(username));
    if (!socketStates.contains(socket)) { // Первый этап: клиент отправил только логин
        if (clientDataBase->existsUser(username)) {
            QMap<QString, QString> userData = clientDataBase->getUserData(username);
            QString salt = userData["salt"];
            PacketServerResponse responseAuth;
            responseAuth.SetResponseType(PacketServerResponse::ServerResponseType::Auth);
            responseAuth.SetResponseStatus(PacketServerResponse::ServerResponseStatus::SuccessUsername);
            responseAuth.SetSalt(salt);
            QByteArray serializedAuth = responseAuth.serialize();
            managerNetwork->sendMessageToUser(socket, serializedAuth);

            socketStates[socket] = {username, true};

            logger.log(QtInfoMsg, QString("Отправляю соль для пользователя %1").arg(username));
        } else {
            PacketServerResponse responseAuth;
            responseAuth.SetResponseType(PacketServerResponse::ServerResponseType::Auth);
            responseAuth.SetResponseStatus(PacketServerResponse::ServerResponseStatus::Failed);
            responseAuth.SetResponseMessage("Вы не зарегестрированы");
            QByteArray serializedAuth = responseAuth.serialize();
            logger.log(QtWarningMsg, QString("Пользователь %1 не найден в базе данных").arg(username));
            managerNetwork->sendMessageToUser(socket, serializedAuth);
        }
    } else { // Второй этап клиент отправил логин и хэш
        AuthState state = socketStates.value(socket);
        if (state.hasSentSalt) {
            QMap<QString, QString> userData = clientDataBase->getUserData(state.username);
            QString Hash = userData["password_hash"];
            logger.log(QtDebugMsg, QString("Проверяю хэш пароля для пользователя %1").arg(state.username));

            if (password == Hash) {
                PacketServerResponse response;
                response.SetResponseType(PacketServerResponse::ServerResponseType::Auth);
                response.SetResponseStatus(PacketServerResponse::ServerResponseStatus::Success);
                QByteArray serialized = response.serialize();
                managerNetwork->sendMessageToUser(socket, serialized);
                logger.log(QtInfoMsg, QString("Аутентификация успешна для пользователя %1").arg(state.username));
                socketStates.remove(socket);
            } else {
                qDebug() << "косяк с паролем";
                PacketServerResponse response;
                response.SetResponseType(PacketServerResponse::ServerResponseType::Auth);
                response.SetResponseStatus(PacketServerResponse::ServerResponseStatus::Failed);
                response.SetResponseMessage("Неправильный пароль");
                logger.log(QtWarningMsg, QString("Аутентификация не удалась для пользователя %1: неправильный пароль").arg(state.username));
                QByteArray serialized = response.serialize();
                managerNetwork->sendMessageToUser(socket, serialized);
                socketStates.remove(socket);
            }
        }
    }
}



PacketMessageHandler::PacketMessageHandler(ClientDataBase* db, ManagerNetwork* managerNetwork, ChatManager* chatManager, QObject* parent)
    : QObject(parent), clientDataBase(db), managerNetwork(managerNetwork), chatManager(chatManager) {}

void PacketMessageHandler::handle(QTcpSocket* socket, PacketMessage& packet) {
    QString chatName = packet.getChatName();
    QString sender = packet.getFrom();
    QString text = packet.getText();

    QMap<QString, QString> userData = clientDataBase->getUserData(sender);
    QString firstName = userData.value("firstName", "Unknown");
    QString lastName = userData.value("lastName", "User");

    PacketMessage mes;
    mes.setFirstName(firstName);
    mes.setLastName(lastName);
    mes.setChatName(chatName);
    mes.setFrom(sender);
    mes.setTimestamp(QDateTime::currentDateTime());
    mes.setText(text);

    Chat* chat = chatManager->getChat(chatName);

    if (chat->getName() == nullptr) {
        return;
    }

    chatManager->addMessageToChat(chatName, sender, text, QDateTime::currentDateTime(), firstName, lastName);

    QByteArray serializedMes = mes.serialize();
    managerNetwork->broadcastMessage(serializedMes);
}


PacketChatListHandler::PacketChatListHandler(ChatManager* manager, ManagerNetwork* managerNetwork, QObject* parent)
    : QObject(parent), chatManager(manager), managerNetwork(managerNetwork) {}

void PacketChatListHandler::handle(QTcpSocket* socket, PacketChatList& packet) {
    QStringList chatNames = chatManager->getAllChatNames();
    packet.setChatNames(chatNames);
    QByteArray serializedData = packet.serialize();
    managerNetwork->sendMessageToUser(socket, serializedData);
}
