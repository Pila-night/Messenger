#pragma once

#include <cstdint>
#include <memory>
#include <QByteArray>
#include <QIODevice>
#include <CRC.h> // Библиотека с https://github.com/d-bahr/CRCpp
#include <QDebug>

enum class PacketType : uint8_t {
    Register,
    Auth,
    Message,
    ServerResponse
};

class Packet {
protected:

    virtual void serializeData(QDataStream& stream) const = 0;
    virtual void deserializeData(QDataStream& stream) = 0;

    static void serializeString(QDataStream& stream, const QString& str);
    static QString deserializeString(QDataStream& stream);

public:

    QByteArray serialize() const;
    static std::unique_ptr<Packet> deserialize(const QByteArray& data);

    virtual PacketType getType() const = 0;
    virtual ~Packet() = default;

private:
    QByteArray crc(const QByteArray& data) const;
};

/*подсчет контрольной суммы.*/
QByteArray Packet::crc(const QByteArray& data) const {
    uint32_t crc = CRC::Calculate(data.constData(), data.size(), CRC::CRC_32());
    QByteArray result;
    QDataStream stream(&result, QIODeviceBase::WriteOnly);
    stream << crc;
    return result;
}

/*Получается пакет вида [Длина, которую необходимо прочитать][Тип][Длина][Данные][Длина][Данные][CRC]*/
QByteArray Packet::serialize() const {
    QByteArray data;
    QDataStream stream(&data, QIODeviceBase::WriteOnly);

    uint8_t type = static_cast<uint8_t>(getType());
    stream << type;

    /*Получаем  [ТИП][Длина][Данные][Длина][Данные]*/
    serializeData(stream);

    QByteArray CRC = crc(data);
    /*Теперь  [ТИП][Длина][Данные][Длина][Данные][CRC]*/
    data.append(CRC);

    QByteArray finalPacket;
    QDataStream headerStream(&finalPacket, QIODeviceBase::WriteOnly);
    uint32_t totalLength = static_cast<uint32_t>(data.size()); // Длина от типа + данные + CRC
    headerStream << totalLength; // Записываем размер, который необходимо прочитать
    finalPacket.append(data); // Добавляем данные
    return finalPacket;
}



void Packet::serializeString(QDataStream& stream, const QString& str) {
    QByteArray utf_8 = str.toUtf8();
    uint16_t length = static_cast<uint16_t>(utf_8.size());
    stream << length; // Записываем длину строки
    stream.writeRawData(utf_8.constData(), length); // Записываем сами данные
}

QString Packet::deserializeString(QDataStream& stream) {
    uint16_t length;
    stream >> length;
    QByteArray data(length, '\0');
    stream.readRawData(data.data(), length);
    return QString::fromUtf8(data);
}


class Register : public Packet {
private:
    QString username;
    QString password;

protected:
    void serializeData(QDataStream& stream) const override {
        serializeString(stream, username);
        serializeString(stream, password);
    }

    void deserializeData(QDataStream& stream) override {
        username = deserializeString(stream);
        password = deserializeString(stream);
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
    void serializeData(QDataStream& stream) const override {
        serializeString(stream, username);
        serializeString(stream, password);
    }

    void deserializeData(QDataStream& stream) override {
        username = deserializeString(stream);
        password = deserializeString(stream);
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
    void serializeData(QDataStream& stream) const override {
        serializeString(stream, from);
        serializeString(stream, text);
    }

    void deserializeData(QDataStream& stream) override {
        from = deserializeString(stream);
        text = deserializeString(stream);
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
    void serializeData(QDataStream& stream) const override {
        serializeString(stream, message);
    }

    void deserializeData(QDataStream& stream) override {
        message = deserializeString(stream);
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
    QDataStream stream(data);

    uint32_t totalLength;
    stream >> totalLength;

    uint8_t typeValue;
    stream >> typeValue;
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
        packet->deserializeData(stream);
    }

    return packet;
}
