#pragma once

#include <cstdint>
#include <memory>
#include <QByteArray>
#include <QIODevice>
#include <QString>
#include "libs\crc\CRC.h"// Библиотека с https://github.com/d-bahr/CRCpp
#include "ByteBuffer.h"
#include <QStringList>
#include <QDateTime>
#include <QTcpSocket>



class PacketHandler;

enum class PacketType : qint8 {
    /*Аутентификация и идентификация*/

    Register, /*регистрация нового пользователя*/
    Auth, /* аутентификация нового пользоватпеля*/

    /********************************/

    ServerResponse, /*ответ сервера*/

    /*Типы пакетов, связанные с чатами*/
    CreateChat, /*создать чат*/
    ChatList, /*получить список чатов*/
    Message, /*сообщение в чат*/
    /**********************************/
};

class Packet {
protected:
    virtual void serializeData(ByteBuffer& buffer) const = 0;
    virtual void deserializeData(ByteBuffer& buffer) = 0;

    static void serializeString(ByteBuffer& buffer, const QString& str);
    static QString deserializeString(ByteBuffer& buffer);

public:
    QByteArray serialize() const;
    static std::shared_ptr<Packet> deserialize(const QByteArray& data);

    virtual PacketType getType() const = 0;
    virtual ~Packet() = default;

    virtual void handle(QTcpSocket* socket, PacketHandler* handler) = 0;

    QString getTypeName() const {
        switch (getType()) {
        case PacketType::Register:       return "Register";
        case PacketType::Message:        return "Message";
        case PacketType::ServerResponse: return "ServerResponse";
        case PacketType::Auth:          return "Auth";
        case PacketType::ChatList:       return "ChatList";
        default:                         return "Unknown";
        }
    }

private:
    QByteArray  crcToByteArray(const ByteBuffer& buffer) const;
    static uint32_t crcToInt32(const QByteArray& data);
};

class PacketRegister : public Packet {
private:
    QString username;
    QString password;
    QString first_name;
    QString last_name;

protected:
    void serializeData(ByteBuffer& buffer) const override;
    void deserializeData(ByteBuffer& buffer) override;

public:
    void handle(QTcpSocket* socket, PacketHandler* handler) override;
    PacketType getType() const override;

    QString getUsername() const;
    void setUsername(const QString &text);

    QString getPassword() const;
    void setPassword(const QString &text);

    QString getFirst_name() const;
    void setFirst_name(const QString &text);

    QString getLast_name() const;
    void setLast_name(const QString &text);


};

class PacketAuth : public Packet {
private:
    QString username;
    QString password;

protected:
    void serializeData(ByteBuffer& buffer) const override;
    void deserializeData(ByteBuffer& buffer) override;

public:
    void handle(QTcpSocket* socket, PacketHandler* handler) override;
    PacketType getType() const override;
    QString getUsername() const;
    void setUsername(const QString &text);
    QString getPassword() const;
    void setPassword(const QString &text);
};





class PacketMessage : public Packet {
private:
    QString firstName;
    QString lastName;
    QString from;
    QString text;
    QDateTime timestamp;
    QString ChatName;


protected:
    void serializeData(ByteBuffer& buffer) const override;
    void deserializeData(ByteBuffer& buffer) override;

public:
    void handle(QTcpSocket* socket, PacketHandler* handler) override;
    PacketType getType() const override;

    QString getFirstName() const;
    void setFirstName(const QString &firstName);

    QString getLastName() const;
    void setLastName(const QString &lastName);

    QString getFrom() const;
    void setFrom(const QString &text);

    QString getText() const;
    void setText(const QString &str);

    QDateTime getTimestamp() const;
    void setTimestamp(const QDateTime &time);

    QString getChatName() const;
    void setChatName(const QString &name);

};

class PacketCreateChat : public Packet {
private:
    QString nameChat;

public:
    void handle(QTcpSocket* socket, PacketHandler* handler) override;
    void serializeData(ByteBuffer& buffer) const override;
    void deserializeData(ByteBuffer& buffer) const;

    PacketType getType() const override
    {
        return PacketType::CreateChat;
    }

    QString getChatName() const {
        return nameChat;
    }

    void setChatName(const QString& name) {
        nameChat = name;
    }

};

class PacketChatList : public Packet {
private:
    QStringList chatNames;

public:
    void handle(QTcpSocket* socket, PacketHandler* handler) override;
    void serializeData(ByteBuffer& buffer) const override;
    void deserializeData(ByteBuffer& buffer) override;
    PacketType getType() const override { return PacketType::ChatList; }

    const QStringList& getChatNames() const { return chatNames; }
    void setChatNames(const QStringList& names) { chatNames.append(names); }

};








class PacketServerResponse : public Packet {

protected:
    void serializeData(ByteBuffer& buffer) const override;
    void deserializeData(ByteBuffer& buffer) override;

public:

    void handle(QTcpSocket* socket, PacketHandler* handler) override;
    enum class ServerResponseType : qint8{
        Auth,
        Register
    };

    enum class ServerResponseStatus : qint8{
        Success,
        SuccessUsername,
        Failed,
    };

    PacketType getType() const override;
    QString  getResponse() const;
    void SetResponseMessage(const QString &text);

    void SetResponseType(const ServerResponseType & type);
    ServerResponseType GetResponseType();

    void SetResponseStatus(const ServerResponseStatus& status);
    ServerResponseStatus GetResponseStatus();

    void SetSalt(const QString& solt){salt = solt;}
    QString const GetSalt(){return salt;}

    bool  getRegisterStatus() const;

private:
    QString message;
    QString salt;
    bool RegisterStatus = false;
    ServerResponseStatus Status;
    ServerResponseType ResponseType;
};

