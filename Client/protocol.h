#pragma once

#include <cstdint>
#include <memory>
#include <QByteArray>
#include <QIODevice>
#include <CRC.h> // Библиотека с https://github.com/d-bahr/CRCpp
#include <QDebug>
#include "ByteBuffer.h"

enum class PacketType : uint8_t {
    Register,
    Auth,
    Message,
    ServerResponse
};

class Packet {
protected:

    virtual void serializeData(ByteBuffer& buffer) const = 0;
    virtual void deserializeData(ByteBuffer& buffer) = 0;

    static void serializeString(ByteBuffer& buffer, const QString& str);
    static QString deserializeString(ByteBuffer& buffer);

public:

    QByteArray serialize() const;
    static std::unique_ptr<Packet> deserialize(const QByteArray& data);

    virtual PacketType getType() const = 0;
    virtual ~Packet() = default;

private:
    QByteArray crc(const ByteBuffer& buffer) const;
};

/*подсчет контрольной суммы.*/
QByteArray Packet::crc(const ByteBuffer& buffer) const {
    uint32_t crc = CRC::Calculate(buffer.constData(), buffer.size(), CRC::CRC_32());
    ByteBuffer result;
    result.writeIntLE(crc);
    return result;
}

/*Получается пакет вида [Длина, которую необходимо прочитать][Тип][Длина][Данные][Длина][Данные][CRC]*/
QByteArray Packet::serialize() const {
    ByteBuffer buf;
    buf.writeByte(static_cast<qint8>(getType()));


    /*Получаем  [ТИП][Длина][Данные][Длина][Данные]*/
    serializeData(buf);

    QByteArray CRC  =  crc(buf);
    buf.write(CRC);

    /*Теперь  [ТИП][Длина][Данные][Длина][Данные][CRC]*/
    ByteBuffer finalPacket;
    finalPacket.writeIntLE(buf.length());
    finalPacket.write(buf.readEnd());

    return finalPacket;
}



void Packet::serializeString(ByteBuffer& buffer, const QString& str) {
    QByteArray utf_8 = str.toUtf8();
    buffer.writeShortLE(utf_8.size()); //записываю длину строки
    buffer.write(utf_8); // записываю саму строку
}

QString Packet::deserializeString(ByteBuffer& buffer) {
    qint16 length = buffer.readShortLE();
    QByteArray data = buffer.read(length);
    return QString::fromUtf8(data);
}


class Register : public Packet {
private:
    QString username;
    QString password;

protected:
    void serializeData(ByteBuffer& buffer) const override {
        serializeString(buffer, username);
        serializeString(buffer, password);
    }

    void deserializeData(ByteBuffer& buffer) override {
        username = deserializeString(buffer);
        password = deserializeString(buffer);
    }

public:
    PacketType getType() const override {
        return PacketType::Register;
    }
    QString getUsername() const {
        return username;
    }
    void setUsername(const QString & text){
        username = text;
    }

    QString getPassword() const{
        return password;
    }
    void setPassword(const QString& text) {
        password = text;
    }
};

class Auth : public Packet {
private:
    QString username;
    QString password;

protected:
    void serializeData(ByteBuffer& buffer) const override {
        serializeString(buffer, username);
        serializeString(buffer, password);
    }

    void deserializeData(ByteBuffer& buffer) override {
        username = deserializeString(buffer);
        password = deserializeString(buffer);
    }

public:
    PacketType getType() const override {
        return PacketType::Auth;
    }
    QString getUsername() const {
        return username;
    }
    void setUsername(const QString & text){
        username = text;
    }

    QString getPassword() const{
        return password;
    }
    void setPassword(const QString& text) {
        password = text;
    }
};

class Message : public Packet {
private:
    QString from;
    QString text;

protected:
    void serializeData(ByteBuffer& buffer) const override {
        serializeString(buffer, from);
        serializeString(buffer, text);
    }

    void deserializeData(ByteBuffer& buffer) override {
        from = deserializeString(buffer);
        text = deserializeString(buffer);
    }

public:
    PacketType getType() const override {
        return PacketType::Message;
    }
    QString getFrom() const {
        return from;
    }
    void setFrom(const QString &text) {
        from = text;
    }

    QString getText() const {
        return text;
    }
    void setText(const QString &str){
        text = str;
    }
};

class ServerResponse : public Packet {
private:
    QString message;

protected:
    void serializeData(ByteBuffer& buffer) const override {
        serializeString(buffer, message);
    }

    void deserializeData(ByteBuffer& buffer) override {
        message = deserializeString(buffer);
    }

public:
    PacketType getType() const override {
        return PacketType::ServerResponse;
    }
    QString getResponse()  const {
        return message;
    }
    void SetResponse(const QString &text) {
        message = text;
    }
};



std::unique_ptr<Packet> Packet::deserialize(const QByteArray& data) {
    ByteBuffer buffer(data);

    qint32 totalLength = buffer.readIntLE();

    qint8 typeValue = buffer.readByte();

    PacketType type = static_cast<PacketType>(typeValue);

    std::unique_ptr<Packet> packet;
    switch (type) {
    case PacketType::Register: {
        packet = std::make_unique<Register>();
        break;
    }
    case PacketType::Auth: {
        packet = std::make_unique<Auth>();
        break;
    }
    case PacketType::Message: {
        packet = std::make_unique<Message>();
        break;
    }
    case PacketType::ServerResponse: {
        packet = std::make_unique<ServerResponse>();
        break;
    }
    default: return nullptr;
    }

    if (packet) {
        packet->deserializeData(buffer);
    }

    return packet;
}
