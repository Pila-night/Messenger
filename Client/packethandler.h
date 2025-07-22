#ifndef PACKETHANDLER_H
#define PACKETHANDLER_H

#include <QDebug>
#include <QObject>
#include "protocol.h"



/*
 Если надумаю делать создание чатов с помощью прав администратора
то не забыть доделать обработку пакетов для PacketCreateChat
*/

class PacketHandler {
public:
    virtual ~PacketHandler() = default;

    virtual void handle(PacketRegister &packet) = 0;
    virtual void handle(PacketMessage &packet) = 0;
    virtual void handle(PacketServerResponse &packet) = 0;
    virtual void handle(PacketChatList& packet) = 0;

private:
    QString salt;
};

class PacketServerResponseHandler : public QObject, public PacketHandler {
    Q_OBJECT

public:
    PacketServerResponseHandler(QObject *parent = nullptr) : QObject(parent) {}

    void handle(PacketRegister& packet) override {}
    void handle(PacketMessage& packet) override {}
    void handle(PacketChatList& packet) override{}
    void handle(PacketServerResponse& packet) override {
        if (packet.GetResponseType() == PacketServerResponse::ServerResponseType::Auth) {
            switch (packet.GetResponseStatus()) {
            case PacketServerResponse::ServerResponseStatus::Success:
                emit authSuccess();
                break;
            case PacketServerResponse::ServerResponseStatus::SuccessUsername:
                emit authSaltReceived(packet.GetSalt());
                break;
            case PacketServerResponse::ServerResponseStatus::Failed:
                emit authFailed(packet.getResponse());
                break;
            default:
                emit authFailed("Получен неизвестный статус авторизации");
                break;
            }
        }

        if (packet.GetResponseType() == PacketServerResponse::ServerResponseType::Register) {
            if (packet.getRegisterStatus()) {
                emit RegisterSuccess();
            } else {
                emit RegisterFailed();
            }
        }
    }

signals:
    void authSaltReceived(const QString& salt);
    void authFailed(const QString& message);
    void authSuccess();
    void RegisterSuccess();
    void RegisterFailed();
};

class PacketMessageHandler : public QObject, public PacketHandler {
    Q_OBJECT

public:
    PacketMessageHandler(QObject* parent = nullptr)
                        : QObject(parent) {}

    void handle(PacketRegister& packet) override {}
    void handle(PacketServerResponse& packet) override {}
    void handle(PacketChatList& packet) override{}


    void handle(PacketMessage& packet) override {
        QString chatName = packet.getChatName();
        QString sender = packet.getFrom();
        QString text = packet.getText();
        QDateTime timestamp = packet.getTimestamp();
        emit messageReceived(chatName, sender, text, timestamp);
    }

signals:
    void messageReceived(const QString& chatName, const QString& sender, const QString& text, const QDateTime& timestamp);
};


class PacketChatListHandler : public QObject, public PacketHandler {
    Q_OBJECT

public:
    PacketChatListHandler(QObject* parent = nullptr)
                         : QObject(parent) {}

    void handle(PacketRegister& packet) override {}
    void handle(PacketMessage& packet) override {}
    void handle(PacketServerResponse& packet) override {}
    void handle(PacketChatList& packet) override{
        QStringList chatList = packet.getChatNames();
        emit chatListReceived(chatList);
};

signals:
    void chatListReceived(const QStringList& chatList);
};



#endif // PACKETHANDLER_H
