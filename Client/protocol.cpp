#include "protocol.h"
#include "ByteBuffer.h"
#include <QDebug>

/*подсчет контрольной суммы.*/
QByteArray Packet::crcToByteArray(const ByteBuffer& buffer) const {
    uint32_t crc = CRC::Calculate(buffer.constData(), buffer.size(), CRC::CRC_32());
    ByteBuffer result;
    result.writeIntLE(crc);
    return result;
}

uint32_t Packet::crcToInt32(const QByteArray& data)  {
    return CRC::Calculate(data.constData(), data.size(), CRC::CRC_32());
}


/**
         * @brief Packet::serialize() сериализует данные
         * @return QByteArray Пакет данных формата
         * [ТИП ПАКЕТА][РАЗМЕР ПОЛЕЗНЫХ ДАННЫХ][CRC][Длина][Данные][Длина][Данные]
         */

QByteArray Packet::serialize() const {

    /*Сериализуются данные, которые находятся в пакете (в зависимости от типа пакета разная сериализация)*/
    ByteBuffer data;
    serializeData(data);

    /*Вычисляется CRC от полезных данных */
    QByteArray CRC  =  crcToByteArray(data);

    /*Теперь  [ТИП][Длина][Данные][Длина][Данные][CRC]*/
    ByteBuffer finalPacket;

    /*Записываем тип заголовка в пакет.*/
    finalPacket.writeByte(static_cast<qint8>(getType()));

    /*Записываем размер полезных данных в пакет*/
    finalPacket.writeIntLE(data.length());

    /*Записываем CRC в пакет*/
    finalPacket.write(CRC);

    /*Записываем сами данные в пакет*/
    finalPacket.write(data);
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

    /*Читаем тип пакета*/
    qint8 typeValue = buffer.readByte();

    /*Читаем размер полезных данных*/
    qint32 usefulDataLength = buffer.readIntLE();

    /*Читаем CRC*/
    qint32 CRC = buffer.readIntLE();

    /*Читаем полезные данные*/
    QByteArray usefulData = buffer.read(usefulDataLength);

    /*пока без обработки ошибок*/
    if(usefulData.size() !=usefulDataLength )
    {
        qDebug() <<"Ошибка при чтении данных";
        return nullptr;
    }

    /*Высчитываем CRC от полезных данных
     чтобы выполнить сравнение с присланным CRC
    */

    qint32 calcCRC = crcToInt32(usefulData);

    /*пока без обработки ошибок*/
    if (calcCRC != CRC){
        qDebug() << "Не совпало CRC";
        return nullptr;
    }

    PacketType type = static_cast<PacketType>(typeValue);

    std::shared_ptr<Packet> packet;
    switch (type) {
    case PacketType::Register:
        packet = std::make_shared<PacketRegister>();
        break;
    case PacketType::Auth:
        packet = std::make_shared<PacketAuth>();
        break;
    case PacketType::Message:
        packet = std::make_shared<PacketMessage>();
        break;
    case PacketType::ServerResponse:
        packet = std::make_shared<PacketServerResponse>();
        break;
    default:
        return nullptr;
    }

    if (packet) {
        packet->deserializeData(buffer);
    }

    return packet;
}

// --- Реализация класса PacketRegister ---

void PacketRegister::serializeData(ByteBuffer& buffer) const {
    Packet::serializeString(buffer, username);
    Packet::serializeString(buffer, password);
}

void PacketRegister::deserializeData(ByteBuffer& buffer) {
    username = Packet::deserializeString(buffer);
    password = Packet::deserializeString(buffer);
}

PacketType PacketRegister::getType() const {
    return PacketType::Register;
}

QString PacketRegister::getUsername() const {
    return username;
}

void PacketRegister::setUsername(const QString &text) {
    username = text;
}

QString PacketRegister::getPassword() const {
    return password;
}

void PacketRegister::setPassword(const QString &text) {
    password = text;
}

// --- Реализация класса PacketAuth ---

void PacketAuth::serializeData(ByteBuffer& buffer) const {
    Packet::serializeString(buffer, username);
    Packet::serializeString(buffer, password);
}

void PacketAuth::deserializeData(ByteBuffer& buffer) {
    username = Packet::deserializeString(buffer);
    password = Packet::deserializeString(buffer);
}

PacketType PacketAuth::getType() const {
    return PacketType::Auth;
}

QString PacketAuth::getUsername() const {
    return username;
}

void PacketAuth::setUsername(const QString &text) {
    username = text;
}

QString PacketAuth::getPassword() const {
    return password;
}

void PacketAuth::setPassword(const QString &text) {
    password = text;
}

// --- Реализация класса PacketMessage ---

void PacketMessage::serializeData(ByteBuffer& buffer) const {
    Packet::serializeString(buffer, from);
    Packet::serializeString(buffer, text);
}

void PacketMessage::deserializeData(ByteBuffer& buffer) {
    from = Packet::deserializeString(buffer);
    text = Packet::deserializeString(buffer);
}

PacketType PacketMessage::getType() const {
    return PacketType::Message;
}

QString PacketMessage::getFrom() const {
    return from;
}

void PacketMessage::setFrom(const QString &text) {
    from = text;
}

QString PacketMessage::getText() const {
    return text;
}

void PacketMessage::setText(const QString &str) {
    text = str;
}

// --- Реализация класса PacketServerResponse ---

void PacketServerResponse::serializeData(ByteBuffer& buffer) const {
    Packet::serializeString(buffer, message);
}

void PacketServerResponse::deserializeData(ByteBuffer& buffer) {
    message = Packet::deserializeString(buffer);
}

PacketType PacketServerResponse::getType() const {
    return PacketType::ServerResponse;
}

QString PacketServerResponse::getResponse() const {
    return message;
}

void PacketServerResponse::SetResponse(const QString &text) {
    message = text;
}

// ---PacketAuthChalleng ---

void PacketAuthChallenge::serializeData(ByteBuffer& buffer) const {
    Packet::serializeString(buffer, salt);
}

void PacketAuthChallenge::deserializeData(ByteBuffer& buffer) {
    salt = Packet::deserializeString(buffer);
}

PacketType PacketAuthChallenge::getType() const {
    return PacketType::AuthChallenge;
}

QString PacketAuthChallenge::getSalt() const {
    return salt;
}

void PacketAuthChallenge::setSalt(const QString &s) {
    salt = s;
}
