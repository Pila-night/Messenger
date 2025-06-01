#include "protocol.h"
#include "ByteBuffer.h"
#include <QDebug>

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
    QByteArray utf8 = str.toUtf8();
    buffer.writeShortLE(utf8.size());
    buffer.write(utf8);
}

QString Packet::deserializeString(ByteBuffer& buffer) {
    qint16 length = buffer.readShortLE();
    QByteArray data = buffer.read(length);
    return QString::fromUtf8(data);
}

std::shared_ptr<Packet> Packet::deserialize(const QByteArray& data) {
    ByteBuffer buffer(data);

    qint32 totalLength = buffer.readIntLE();

    qint8 typeValue = buffer.readByte();
    PacketType type = static_cast<PacketType>(typeValue);

    std::shared_ptr<Packet> packet;
    switch (type) {
    case PacketType::Register:
        packet = std::make_shared<Register>();
        break;
    case PacketType::Auth:
        packet = std::make_shared<Auth>();
        break;
    case PacketType::Message:
        packet = std::make_shared<Message>();
        break;
    case PacketType::ServerResponse:
        packet = std::make_shared<ServerResponse>();
        break;
    default:
        return nullptr;
    }

    if (packet) {
        packet->deserializeData(buffer);
    }

    return packet;
}

// --- Реализация класса Register ---

void Register::serializeData(ByteBuffer& buffer) const {
    Packet::serializeString(buffer, username);
    Packet::serializeString(buffer, password);
}

void Register::deserializeData(ByteBuffer& buffer) {
    username = Packet::deserializeString(buffer);
    password = Packet::deserializeString(buffer);
}

PacketType Register::getType() const {
    return PacketType::Register;
}

QString Register::getUsername() const {
    return username;
}

void Register::setUsername(const QString &text) {
    username = text;
}

QString Register::getPassword() const {
    return password;
}

void Register::setPassword(const QString &text) {
    password = text;
}

// --- Реализация класса Auth ---

void Auth::serializeData(ByteBuffer& buffer) const {
    Packet::serializeString(buffer, username);
    Packet::serializeString(buffer, password);
}

void Auth::deserializeData(ByteBuffer& buffer) {
    username = Packet::deserializeString(buffer);
    password = Packet::deserializeString(buffer);
}

PacketType Auth::getType() const {
    return PacketType::Auth;
}

QString Auth::getUsername() const {
    return username;
}

void Auth::setUsername(const QString &text) {
    username = text;
}

QString Auth::getPassword() const {
    return password;
}

void Auth::setPassword(const QString &text) {
    password = text;
}

// --- Реализация класса Message ---

void Message::serializeData(ByteBuffer& buffer) const {
    Packet::serializeString(buffer, from);
    Packet::serializeString(buffer, text);
}

void Message::deserializeData(ByteBuffer& buffer) {
    from = Packet::deserializeString(buffer);
    text = Packet::deserializeString(buffer);
}

PacketType Message::getType() const {
    return PacketType::Message;
}

QString Message::getFrom() const {
    return from;
}

void Message::setFrom(const QString &text) {
    from = text;
}

QString Message::getText() const {
    return text;
}

void Message::setText(const QString &str) {
    text = str;
}

// --- Реализация класса ServerResponse ---

void ServerResponse::serializeData(ByteBuffer& buffer) const {
    Packet::serializeString(buffer, message);
}

void ServerResponse::deserializeData(ByteBuffer& buffer) {
    message = Packet::deserializeString(buffer);
}

PacketType ServerResponse::getType() const {
    return PacketType::ServerResponse;
}

QString ServerResponse::getResponse() const {
    return message;
}

void ServerResponse::SetResponse(const QString &text) {
    message = text;
}

// --- AuthChalleng ---

void AuthChallenge::serializeData(ByteBuffer& buffer) const {
    Packet::serializeString(buffer, salt);
}

void AuthChallenge::deserializeData(ByteBuffer& buffer) {
    salt = Packet::deserializeString(buffer);
}

PacketType AuthChallenge::getType() const {
    return PacketType::AuthChallenge;
}

QString AuthChallenge::getSalt() const {
    return salt;
}

void AuthChallenge::setSalt(const QString &s) {
    salt = s;
}
