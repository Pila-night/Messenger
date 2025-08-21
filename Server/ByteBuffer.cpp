#include "ByteBuffer.h"
#include <QString>
#include "exception/ParsingException.h"

/**
 * @brief ByteBuffer - базовый конструктор
 */
ByteBuffer::ByteBuffer() : QByteArray() {
    this->position = 0;
}
/**
 * @brief ByteBuffer - конструктор с указанием размера буфера
 * Но вообще, на методы записи он вроде как сам расширяется
 * @param size - размер требуемого буфера
 */
ByteBuffer::ByteBuffer(quint32 size) : QByteArray() {
    this->resize(size);
    this->position = 0;
}
/**
 * @brief ByteBuffer - конструктор копирования по сути,
 * хоть для С++ он таким не является по синтаксису
 * @param other - второй буффер
 */
ByteBuffer::ByteBuffer(QByteArray other) : QByteArray(other) {
    this->position = 0;
}
/**
 * @brief ByteBuffer - конструктор для строк, чтобы
 * можно было сразу сделать буфер из строчки
 * Я не понимаю почему этого нет в QByteArray
 * @param buffer - строка текста содержая данные
 */
ByteBuffer::ByteBuffer(QString buffer) : QByteArray() {
    this->append(buffer.toLocal8Bit());
    this->position = 0;
}
/**
 * @brief ByteBuffer - конструктор копирования, который
 * является синтаксически таковым для компилятора
 * @param other - объект, который надо скопировать
 */
ByteBuffer::ByteBuffer(ByteBuffer & other) : QByteArray(other) {
    this->position = other.position;
}
//**********************************************************************************************
//********************************              ************************************************
//********************************    Чтение    ************************************************
//********************************              ************************************************
//**********************************************************************************************

/**
 * @brief readByte - чтение одного байта из буфера;
 * Сдвигает индекс чтения буффера на 1 позицию;
 * Нет разницы LE или BE, он же байт :\
 * @return
 */
quint8 ByteBuffer::readByte() {
    if (position >= this->size()) {
        throw ParsingException("[readChar] Out of bound");
    }
    quint8 s = (quint8)(this->QByteArray::operator[](position++));
    return s;
}
/**
 * @brief readShortLE - чтение 2-х байт из буфера;
 * Сдвигает индекс чтения буффера на 2-ую позицию;
 * Читает в формате LittleEndian
 * @return
 */
qint16 ByteBuffer::readShortLE() {
    if (position + 1 >= this->size()) {
        throw ParsingException("[readShort] Out of bound");
    }
    qint16 s = ((quint8)(this->QByteArray::operator[](position + 0) & 0XFF)) << 0
             | ((quint8)(this->QByteArray::operator[](position + 1) & 0XFF)) << 8;
    position += 2;
    return s;
}
/**
 * @brief readIntLE - чтение 4-х байт из буфера;
 * Сдвигает индекс чтения буффера на 4-ую позицию;
 * Читает в формате LittleEndian
 * @return
 */
qint32 ByteBuffer::readIntLE() {
    if (position + 3 >= this->size()) {
        throw ParsingException("[readInt] Out of bound");
    }
    qint32 s = ((quint8)(this->QByteArray::operator[](position + 0) & 0XFF)) << 0
             | ((quint8)(this->QByteArray::operator[](position + 1) & 0XFF)) << 8
             | ((quint8)(this->QByteArray::operator[](position + 2) & 0XFF)) << 16
             | ((quint8)(this->QByteArray::operator[](position + 3) & 0XFF)) << 24;
    position += sizeof (qint32);
    return s;
}
/**
 * @brief readLongLE - чтение 8-ми байт из буфера;
 * Сдвигает индекс чтения буффера на 8-ую позицию;
 * Читает в формате LittleEndian
 * @return
 */
qint64 ByteBuffer::readLongLE() {

    if (position + 7 >= this->size()) {
        throw ParsingException("[readLong] Out of bound");
    }
    qint64 value = ((quint64)(quint8)(this->QByteArray::operator[](position + 0) & 0XFF)) << 0
             | ((quint64)(quint8)(this->QByteArray::operator[](position + 1) & 0XFF)) << 8
             | ((quint64)(quint8)(this->QByteArray::operator[](position + 2) & 0XFF)) << 16
             | ((quint64)(quint8)(this->QByteArray::operator[](position + 3) & 0XFF)) << 24
             | ((quint64)(quint8)(this->QByteArray::operator[](position + 4) & 0XFF)) << 32
             | ((quint64)(quint8)(this->QByteArray::operator[](position + 5) & 0XFF)) << 40
             | ((quint64)(quint8)(this->QByteArray::operator[](position + 6) & 0XFF)) << 48
             | ((quint64)(quint8)(this->QByteArray::operator[](position + 7) & 0XFF)) << 56;
    position += sizeof (qint64);

    return value;
}
/**
 * @brief readShortBE - чление 2-х байт из буфера;
 * сдвигает индекс чтения буффера на 4-ую позицию;
 * Читает в формате BigEndian
 * @return
 */
qint16 ByteBuffer::readShortBE() {
    if (position + 1 >= this->size()) {
        throw ParsingException("[readShort] Out of bound");
    }
    qint16 s = ((quint8)(this->QByteArray::operator[](position + 1) & 0XFF)) << 0
             | ((quint8)(this->QByteArray::operator[](position + 0) & 0XFF)) << 8;
    position += 2;
    return s;
}
/**
 * @brief readIntBE - чтение 4-х байт из буфера;
 * Сдвигает индекс чтения буффера на 4-ую позицию;
 * Читает в формате BigEndian
 * @return
 */
qint32 ByteBuffer::readIntBE() {
    if (position + 3 >= this->size()) {
        throw ParsingException("[readInt] Out of bound");
    }
    qint32 s = ((qint32)(this->QByteArray::operator[](position + 3) & 0XFF)) << 0
             | ((qint32)(this->QByteArray::operator[](position + 2) & 0XFF)) << 8
             | ((qint32)(this->QByteArray::operator[](position + 1) & 0XFF)) << 16
             | ((qint32)(this->QByteArray::operator[](position + 0) & 0XFF)) << 24;
    position += sizeof (qint32);
    return s;
}
/**
 * @brief readLongBE - чтение 8-ми байт из буфера;
 * сдвигает индекс чтения буффера на 8-ую позицию;
 * Читает в формате BigEndian
 * @return
 */
qint64 ByteBuffer::readLongBE() {
    if (position + 7 >= this->size()) {
        throw ParsingException("[readLong] Out of bound");
    }
    qint64 value = ((quint64)(quint8)(this->QByteArray::operator[](position + 7) & 0XFF)) << 0
             | ((quint64)(quint8)(this->QByteArray::operator[](position + 6) & 0XFF)) << 8
             | ((quint64)(quint8)(this->QByteArray::operator[](position + 5) & 0XFF)) << 16
             | ((quint64)(quint8)(this->QByteArray::operator[](position + 4) & 0XFF)) << 24
             | ((quint64)(quint8)(this->QByteArray::operator[](position + 3) & 0XFF)) << 32
             | ((quint64)(quint8)(this->QByteArray::operator[](position + 2) & 0XFF)) << 40
             | ((quint64)(quint8)(this->QByteArray::operator[](position + 1) & 0XFF)) << 48
             | ((quint64)(quint8)(this->QByteArray::operator[](position + 0) & 0XFF)) << 56;
    position += sizeof (qint64);
    return value;
}
/**
 * @brief read - читает указанное количество байт и возвращает
 * их как объект QByteArray (UP: может возвращать ByteBuffer?)
 * @param lenght - длина зоны, которую можно прочитать
 * @return
 */
QByteArray ByteBuffer::read(qint32 lenght) {
    if (position + lenght > this->length()) {
        throw ParsingException("[readLong] Out of bound");
    }

    QByteArray arr = this->mid(position, lenght);
    position+= lenght;
    return arr;
}
/**
 * @brief readEnd - читает весь оставшийся буффер и возвращает
 * его как объект QByteArray (UP: может возвращать ByteBuffer?)
 * @return
 */
QByteArray ByteBuffer::readEnd() {
    QByteArray buf = this->mid(position, this->length() - position);
    position+= buf.length();
    return buf;
}
//**********************************************************************************************
//********************************              ************************************************
//********************************    Запись    ************************************************
//********************************              ************************************************
//**********************************************************************************************

/**
 * @brief writeByte записывает байт в конец буфера
 * @param num - значение
 * @return обновлённый буфер
 */
ByteBuffer ByteBuffer::writeByte(qint8 num) {
    this->append(num >> 0);
    return *this;
}
/**
 * @brief writeIntLE записывает 4 байта в конец буфера
 * в формате LittleEndian
 * @param num - значение
 * @return обновлённый буфер
 */
ByteBuffer ByteBuffer::writeIntLE(qint32 num) {
    this->append((num >> 0) & 0xFF);
    this->append((num >> 8) & 0xFF);
    this->append((num >> 16) & 0xFF);
    this->append((num >> 24) & 0xFF);
    return *this;
}
/**
 * @brief writeLongLE записывает 8 байт в конец буфера
 * в формате LittleEndian
 * @param num - значение
 * @return обновлённый буфер
 */
ByteBuffer ByteBuffer::writeLongLE(qint64 num) {
    this->append((num >> 0) & 0xFF);
    this->append((num >> 8) & 0xFF);
    this->append((num >> 16) & 0xFF);
    this->append((num >> 24) & 0xFF);
    this->append((num >> 32) & 0xFF);
    this->append((num >> 40) & 0xFF);
    this->append((num >> 48) & 0xFF);
    this->append((num >> 56) & 0xFF);
    return *this;

}
/**
 * @brief writeShortLE записывает 2 байта в конец буфера
 * в формате LittleEndian
 * @param num - значение
 * @return обновлённый буфер
 */
ByteBuffer ByteBuffer::writeShortLE(qint16 num) {
    this->append((num >> 0) & 0xFF);
    this->append((num >> 8) & 0xFF);
    return *this;
}
/**
 * @brief writeIntBE записывает 4 байта в конец буфера
 * в формате BigEndian
 * @param num - значение
 * @return обновлённый буфер
 */
ByteBuffer ByteBuffer::writeIntBE(qint32 num) {
    this->append((num >> 24) & 0xFF);
    this->append((num >> 16) & 0xFF);
    this->append((num >> 8) & 0xFF);
    this->append((num >> 0) & 0xFF);
    return *this;
}
/**
 * @brief writeLongBE записывает 8 байт в конец буфера
 * в формате BigEndian
 * @param num - значение
 * @return обновлённый буфер
 */
ByteBuffer ByteBuffer::writeLongBE(qint64 num) {
    this->append((num >> 56) & 0xFF);
    this->append((num >> 48) & 0xFF);
    this->append((num >> 40) & 0xFF);
    this->append((num >> 32) & 0xFF);
    this->append((num >> 24) & 0xFF);
    this->append((num >> 16) & 0xFF);
    this->append((num >> 8) & 0xFF);
    this->append((num >> 0) & 0xFF);
    return *this;
}
/**
 * @brief writeShortBE записывает 2 байта в конец буфера
 * в формате BigEndian
 * @param num - значение
 * @return обновлённый буфер
 */
ByteBuffer ByteBuffer::writeShortBE(qint16 num) {
    this->append((num >> 8) & 0xFF);
    this->append((num >> 0) & 0xFF);
    return *this;
}
/**
 * @brief write записывает буфер в конец текущего буфера
 * по сути конкатенация буферов
 * @param buf - буфер, который следует дописать в конец
 * @return склеенный буфер
 */
ByteBuffer ByteBuffer::write(QByteArray buf) {
    this->append(buf);
    return *this;
}

//**********************************************************************************************
//********************************              ************************************************
//********************************  Служебное   ************************************************
//********************************              ************************************************
//**********************************************************************************************

/**
 * @brief isEnd - проверяет, можно ли ещё читать байты
 * @return true - если больше нельзя читать буфер
 *          false если буфер ещё не закончился
 */
bool ByteBuffer::isEnd() {
    return position == this->length()-1;
}
/**
 * @brief flushIndex - сбрасывает индекс чтения на 0
 * что позволяет читать буфер с самого начала
 * @return
 */
void ByteBuffer::flushIndex() {
    this->position = 0;
}
/**
 * @brief clear - сбрасывает содержимое и индексы
 * буфера в исходное состояние
 */
void ByteBuffer::clear() {
    this->QByteArray::clear();
    this->position = 0;
}
/**
 * @brief getAvailableBytes - возвращет количество
 * доступные для чтения байт
 * @return
 */
quint32 ByteBuffer::getAvailableBytes() {
    if (this->length() < this->position) {
        return 0;
    }
    return this->length() - this->position;
}

