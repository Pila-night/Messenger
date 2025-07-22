#include "Chat.h"

/**
 * @brief Chat - конструктор класса Chat.
 * Создает новый объект чата с указанным именем.
 * @param chatName - имя чата.
 */
Chat::Chat(const QString& chatName) : name(chatName) {}


/**
 * @brief addMessage - добавляет новое сообщение в чат.
 * @param sender - отправитель сообщения.
 * @param text - текст сообщения.
 * @param timestamp - временная метка сообщения.
 */
void Chat::addMessage(const QString& sender, const QString& text, const QDateTime& timestamp) {
    messages.append(std::make_shared<Message>(sender, text, timestamp));
}

/**
 * @brief getMessages - возвращает список всех сообщений в чате.
 * @return Список сообщений в виде QList<std::shared_ptr<Message>>.
 */
const QList<std::shared_ptr<Message>>& Chat::getMessages() const {
    return messages;
}

/**
 * @brief getName - возвращает имя чата.
 * @return Имя чата в виде QString.
 */
QString Chat::getName() const {
    return name;
}
