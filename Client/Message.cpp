#include "Message.h"

/**
 * @brief Message - конструктор класса Message.
 * @param sender Отправитель сообщения.
 * @param text Текст сообщения
 * @param timestamp Временная метка сообщения.
 */
Message::Message(const QString& sender, const QString& text, const QDateTime& timestamp)
    : sender(sender), text(text), timestamp(timestamp) {}

/**
 * @brief getSender - возвращает имя отправител сообщения.
 * @return Имя отправителя в виде QString.
 */
QString Message::getSender() const {
    return sender;
}

/**
 * @brief setSender - устанавливает отправителя сообщения.
 * @param sender Имя отправителя
 */
void Message::setSender(const QString& s) {
    sender = s;
}

/**
 * @brief getText - возвращает текст сообщения
 * @return Текст сообщения в виде QString
 */
QString Message::getText() const {
    return text;
}

/**
 * @brief setText - устанавливает текст сообщения
 * @param text Текст сообщения
 */
void Message::setText(const QString& t) {
    text = t;
}

/**
 * @brief getTimestamp - возвращает временную метку сообщения.
 * @return Временная метка в виде QDateTime
 */
QDateTime Message::getTimestamp() const {
    return timestamp;
}

/**
 * @brief setTimestamp - устанавливает временную метку сообщения
 * @param ts Новая временная метка
 */
void Message::setTimestamp(const QDateTime& ts) {
    timestamp = ts;
}
