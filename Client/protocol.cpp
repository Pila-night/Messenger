#include "protocol.h"
#include "packethandler.h"
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
    qint16 size = utf8.size();
    buffer.writeShortLE(size);
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
    case PacketType::Message:
        packet = std::make_shared<PacketMessage>();
        break;
    case PacketType::ServerResponse:
        packet = std::make_shared<PacketServerResponse>();
        break;
    case PacketType::Auth:
        packet = std::make_shared<PacketAuth>();
        break;
    /*case PacketType::CreateChat:
        packet = std::make_shared<PacketCreateChat>();
        break;*/
    case PacketType::ChatList :
        packet = std::make_shared<PacketChatList>();
        break;
    default:
        return nullptr;
    }


    ByteBuffer usefulBuf(usefulData); /*содержит полезные данные*/
    if (packet) {
        packet->deserializeData(usefulBuf);
    }
    return packet;
}

// --- Реализация класса PacketRegister ---

void PacketRegister::serializeData(ByteBuffer& buffer) const {
    Packet::serializeString(buffer, username);
    Packet::serializeString(buffer, password);
    Packet::serializeString(buffer, first_name);
    Packet::serializeString(buffer, last_name);
}

void PacketRegister::deserializeData(ByteBuffer& buffer) {
    username = Packet::deserializeString(buffer);
    password = Packet::deserializeString(buffer);
    first_name =Packet::deserializeString(buffer);
    last_name =Packet::deserializeString(buffer);
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

QString PacketRegister::getFirst_name() const {
    return first_name;
}

void PacketRegister::setFirst_name(const QString &text) {
    first_name = text;
}

QString PacketRegister::getLast_name() const {
    return last_name;
}

void PacketRegister::setLast_name(const QString &text) {
    last_name = text;
}

void PacketRegister::handle(PacketHandler* handler) {
    if (handler) {
        handler->handle(*this);
    }
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
void PacketAuth::handle(PacketHandler* handler) {
    if (handler) {
        handler->handle(*this);
    }
}


// --- Реализация класса PacketMessage ---

void PacketMessage::serializeData(ByteBuffer& buffer) const {
    Packet::serializeString(buffer, firstName);
    Packet::serializeString(buffer, lastName);
    Packet::serializeString(buffer, from);
    Packet::serializeString(buffer, text);
    Packet::serializeString(buffer, ChatName);
    Packet::serializeString(buffer, timestamp.toString(Qt::ISODate));
}

void PacketMessage::deserializeData(ByteBuffer& buffer) {
    firstName = Packet::deserializeString(buffer);
    lastName = Packet::deserializeString(buffer);
    from = Packet::deserializeString(buffer);
    text = Packet::deserializeString(buffer);
    ChatName = Packet::deserializeString(buffer);
    QString tsStr = Packet::deserializeString(buffer);
    timestamp = QDateTime::fromString(tsStr, Qt::ISODate);
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

QDateTime PacketMessage::getTimestamp() const {
    return timestamp;
}

void PacketMessage::setTimestamp(const QDateTime &time) {
    timestamp = time;
}

QString PacketMessage::getChatName() const {
    return ChatName;
}

void PacketMessage::setChatName(const QString &name) {
    ChatName = name;
}

QString PacketMessage::getFirstName() const {
    return firstName;
}

void PacketMessage::setFirstName(const QString &firstName) {
    this->firstName = firstName;
}

QString PacketMessage::getLastName() const {
    return lastName;
}

void PacketMessage::setLastName(const QString &lastName) {
    this->lastName = lastName;
}


void PacketMessage::handle(PacketHandler* handler) {
    if (handler) {
        handler->handle(*this);
    }
}


// --- Реализация класса PacketServerResponse ---

void PacketServerResponse::serializeData(ByteBuffer& buffer) const  {
    /*Записываем байт типа ответа*/
    buffer.writeByte(static_cast<qint8>(ResponseType));
    /*Записываем байт статуса ответа*/
    buffer.writeByte(static_cast<qint8>(Status));

    if (ResponseType == ServerResponseType::Auth && Status == ServerResponseStatus::Success){
        Packet::serializeString(buffer, salt);
    } else {
        Packet::serializeString(buffer, message);

    }
}

void PacketServerResponse::deserializeData(ByteBuffer& buffer) {
    /*читаем байт типа ответа*/
    ResponseType = static_cast<ServerResponseType>(buffer.readByte());
    /*читаем байт типа  статуса*/
    Status = static_cast<ServerResponseStatus>(buffer.readByte());

    if (ResponseType == ServerResponseType::Auth && Status == ServerResponseStatus::SuccessUsername){
        salt = Packet::deserializeString(buffer); //если у нас был запрос на авторизацию и сервер нашел пользователя в БД с таким Логином, то значит сервер сразу прислал и соль
    }

    else if(ResponseType == ServerResponseType::Register && Status == ServerResponseStatus::Success) {
        RegisterStatus = true;
    }

    else {
        message = Packet::deserializeString(buffer);
    }
}

void PacketServerResponse::SetResponseType(const ServerResponseType& type) {
    ResponseType = type;
}

PacketServerResponse::ServerResponseType PacketServerResponse::GetResponseType()  {
    return ResponseType;
}

void PacketServerResponse::SetResponseStatus(const ServerResponseStatus& status) {
    Status = status;
}
PacketServerResponse::ServerResponseStatus PacketServerResponse::GetResponseStatus()  {
    return Status;
}

PacketType PacketServerResponse::getType() const {
    return PacketType::ServerResponse;
}

QString PacketServerResponse::getResponse() const{
    return message;
}
bool PacketServerResponse::getRegisterStatus() const {
    return RegisterStatus;
}
void PacketServerResponse::SetResponseMessage(const QString &text) {
    message = text;
}

void PacketServerResponse::handle(PacketHandler* handler) {
    if (handler) {
        handler->handle(*this);
    }
}

/************************************/



/* Реализовать позже   / класс PacketCreateChat
void PacketCreateChat::serializeData(ByteBuffer& buffer) const
{
    Packet::serializeString(buffer, nameChat);
}

void PacketCreateChat::deserializeData(ByteBuffer& buffer) const
{
    nameChat = Packet::deserializeString(buffer);
}
*/

// --- PacketChatList ---

void PacketChatList::serializeData(ByteBuffer& buffer) const
{
    buffer.writeShortLE(chatNames.size());
    for (const QString& name : chatNames) {
        Packet::serializeString(buffer, name);
    }
}

void PacketChatList::deserializeData(ByteBuffer& buffer)
{
    qint16 count = buffer.readShortLE();
    chatNames.clear();
    for (int i = 0; i < count; ++i) {
        chatNames.append(Packet::deserializeString(buffer));
    }
}

void PacketChatList::handle(PacketHandler* handler) {
    if (handler) {
        handler->handle(*this);
    }
}
