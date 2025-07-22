#include "ChatManager.h"

/**
     * @brief Конструктор класса ChatManager.
     * @param listModel Модель для отображения списка чатов.
     */
ChatManager::ChatManager(QStandardItemModel* listModel, QObject* parent)
    : QObject(parent), model(listModel) {
    database = new ChatDatabase(this);
    if (!database->open("chats.db")) {
        qWarning() << "Не удалось открыть базу данных.";
    }
}

/**
 * @brief createChat - создает новый чат.
 * Если чат с таким именем уже существует, он не будет создан повторно.
 * @param name Имя нового чата.
 */
void ChatManager::createChat(const QString& name) {
    if (!chats.contains(name)) {
        chats[name] = std::make_shared<Chat>(name);
        auto* item = new QStandardItem(name);
        model->appendRow(item);

        QList<QMap<QString, QString>> messages = database->getMessages(name);
        for (const auto& msg : messages) {
            QDateTime timestamp = QDateTime::fromString(msg["timestamp"], Qt::ISODate);
            chats[name]->addMessage(msg["sender"], msg["text"], timestamp);
        }
    }
}

/**
 * @brief getChat - возвращает объект чата по его имени.
 * @param name Имя чата.
 * @return Указатель на объект чата или nullptr, если чат не найден.
 *
 */
std::shared_ptr<Chat> ChatManager::getChat(const QString& name) {
    return chats.value(name);
}

/**
 * @brief addMessageToChat - добавляет новое сообщение в указанный чат.
 * Сообщение также сохраняется в базе данных.
 * @param chatName Имя чата.
 * @param sender Отправитель сообщения.
 * @param text Текст сообщения.
 * @param timestamp Временная метка сообщения.
 */
void ChatManager::addMessageToChat(const QString& chatName, const QString& sender, const QString& text, const QDateTime& timestamp) {
    auto chat = getChat(chatName);
    if (chat) {
        chat->addMessage(sender, text, timestamp);

        database->addMessage(chatName, sender, text, timestamp);

        emit chatUpdated(chatName);
    }
}

/**
 * @brief ChatManager::getAllChatNames - возвращает список имен всех чатов.
 * @return Список имен чатов.
 */
QStringList ChatManager::getAllChatNames() const {
    return chats.keys();
}
