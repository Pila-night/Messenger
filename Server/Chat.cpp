#include "Chat.h"

/**
 * @brief Конструктор по умолчанию.
 * Создает чат с пустым именем.
 */
Chat::Chat() : name("") {}

/**
 * @brief Конструктор с параметром имени чата.
 * @param chatName Имя чата.
 */
Chat::Chat(const QString& chatName) {
    if (chatName.isEmpty()) {
        name = "UnnamedChat";
    } else {
        name = chatName;
    }
}

/**
 * @brief Добавляет новое сообщение в чат.
 * @param sender Отправитель сообщения.
 * @param text Текст сообщения.
 * @param timestamp Временная метка сообщения.
 * @param firstName Имя отправителя.
 * @param lastName Фамилия отправителя.
 */
void Chat::addMessage(const QString& sender, const QString& text, const QDateTime& timestamp,
                      const QString& firstName, const QString& lastName) {
    messages.append(Message(sender, text, timestamp, firstName, lastName));
}

/**
 * @brief Возвращает список всех сообщений в чате.
 * @return Список сообщений (QList<Message>).
 */
const QList<Message>& Chat::getMessages() const {
    return messages;
}

/**
 * @brief Возвращает имя чата.
 * @return Имя чата.
 */
QString Chat::getName() const {
    return name;
}
