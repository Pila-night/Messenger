#pragma once

#include <cstdint>
/*#include <vector>
#include <memory>*/
#include <QByteArray>
#include <QIODevice>
#include <CRC.h> //Библиотека с https://github.com/d-bahr/CRCpp

enum class PacketType : uint8_t {
    Register,
    Auth,
    Message,
    ServerResponse
};

class Packet
{
protected:
    virtual void serializeData(QByteArray& out) const = 0;
    static void serializeString(QDataStream& stream, const QString& str);

public:
    QByteArray serialize() const;
    virtual PacketType getType() const = 0;
    virtual ~Packet() = default;
    QByteArray crc(const QByteArray& data) const ;
};

QByteArray Packet::crc(const QByteArray& data) const{
    uint32_t crc = CRC::Calculate(data.constData(), data.size(), CRC::CRC_32());
    QByteArray result;
    QDataStream stream(&result, QIODeviceBase::WriteOnly);
    stream << crc;
    return result;
}


/*ФОРМАТ [длина того, что требуется прочитать][лина][ДанныеТип][Д][Длина][Данные][CRC] */
QByteArray Packet::serialize() const{
    QByteArray data;
    QDataStream stream(&data, QIODeviceBase::WriteOnly );

    uint8_t type = static_cast<uint8_t>(getType());
    stream << type;

    serializeData(data);
    QByteArray CRC = crc(data);
    data.append(CRC);

    QByteArray finalPacket;
    QDataStream headerStream(&finalPacket, QIODeviceBase::WriteOnly);
    uint32_t totalLength = static_cast<uint32_t>(data.size()); /*беру длину от типа+данные+crc*/
    headerStream << totalLength; /*записали размер, который необходимо прочитать*/
    finalPacket.append(data); /*добавляю данные*/
    return finalPacket;
}


void Packet::serializeString(QDataStream& stream, const QString& str){
    QByteArray  utf_8= str.toUtf8();
    uint16_t length = static_cast<uint16_t>(utf_8.size());
    stream << length; /* записываю длину, что бы сервер знал сколько байт дальше прочитать*/
    stream.writeRawData(utf_8.constData(), length); /*сами данные*/
}

class Register : Packet {
protected:
    void serializeData(QByteArray& out) const override {
        QDataStream stream(&out, QIODeviceBase::WriteOnly | QIODeviceBase::Append);
        serializeString(stream, username);
        serializeString(stream, password);
    }

public:
    QString username;
    QString password;

    PacketType getType() const override {
        return PacketType::Register;

    }
};


class Auth : Packet{
protected:
    void serializeData(QByteArray& out) const override  {
        QDataStream stream(&out, QIODeviceBase::WriteOnly);
        serializeString(stream,username);
        serializeString(stream,password);
    }
public:
    QString username;
    QString password;

};

class Message : Packet {
protected:
    void serializeData(QByteArray& out) const override {
        QDataStream stream(&out, QIODeviceBase::WriteOnly);
        serializeString(stream, from);
        serializeString(stream, text);

    }
public:
    QString from;
    QString text;

};

class ServerResponse : Packet{
protected:
    void serializeData(QByteArray &out) {
        QDataStream stream(&out, QIODeviceBase::WriteOnly);
        serializeString(stream, message);
    }
public:
    QString message;

};
