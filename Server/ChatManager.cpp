#include "ChatManager.h"
#include <QSqlQuery>
#include "logger.h"

/**
 * @brief Конструктор класса ChatManager.
 * Инициализирует менеджер чатов и загружает существующие чаты из базы данных.
 * @param dbPath Путь к базе данных.
 * @param parent Родительский объект.
 */
ChatManager::ChatManager(const QString& dbPath, QObject* parent)
    : QObject(parent), model(this) {
    Logger& logger = Logger::getInstance();
    if (!database.open(dbPath)) {
        logger.log(QtCriticalMsg, "Не удалось открыть базу данных чатов");
        return;
    }
    QStringList chatNames = database.getAllChatNames();
    for (const QString& name : chatNames) {
        loadChat(name);
    }
}

/**
 * @brief Создает новый чат и добавляет его в базу данных.
 * @param name Имя нового чата.
 */
void ChatManager::createChat(const QString& name) {
    if (!chats.contains(name)) {
        chats.insert(name, Chat(name));
        auto* item = new QStandardItem(name);
        model.appendRow(item);
        database.addChat(name);
        emit chatAdded();
    }
}

/**
 * @brief Возвращает указатель на чат по имени.
 * @param name Имя чата.
 * @return Указатель на чат или nullptr, если чат не найден.
 */
Chat* ChatManager::getChat(const QString& name) {
    QMap<QString, Chat>::iterator it = chats.find(name);
    if (it != chats.end()) {
        return &it.value();
    }
    return nullptr;
}

/**
 * @brief Добавляет сообщение в указанный чат.
 * @param chatName Имя чата.
 * @param sender Отправитель сообщения.
 * @param text Текст сообщения.
 * @param timestamp Временная метка.
 * @param firstName Имя отправителя.
 * @param lastName Фамилия отправителя.
 */
void ChatManager::addMessageToChat(const QString& chatName, const QString& sender, const QString& text,
                                   const QDateTime& timestamp, const QString& firstName, const QString& lastName) {
    Chat* chat = getChat(chatName);
    if (chat) {
        chat->addMessage(sender, text, timestamp, firstName, lastName);
        database.addMessage(chatName, sender, text, timestamp, firstName, lastName);
        emit chatUpdated(chatName);
    }
}

/**
 * @brief Загружает чат из базы данных.
 * @param name Имя чата.
 */
void ChatManager::loadChat(const QString& name) {
    if (!chats.contains(name)) {
        Chat chat(name);
        QList<QMap<QString, QString>> messages = database.getMessages(name);
        for (const auto& msg : messages) {
            QDateTime timestamp = QDateTime::fromString(msg["timestamp"], Qt::ISODate);
            chat.addMessage(msg["sender"], msg["text"], timestamp, msg["firstName"], msg["lastName"]);
        }

        chats.insert(name, chat);
        auto* item = new QStandardItem(name);
        model.appendRow(item);
    }
}


QStringList ChatManager::getAllChatNames() const {
    return chats.keys();
}

/**
 * @brief Удаляет чат из системы, включая его из базы данных и внутренних структур данных.
 * @param name Имя чата для удаления.
 * @return true, если чат успешно удален, иначе false.
 */
bool ChatManager::deleteChat(const QString& name) {
    Logger& logger = Logger::getInstance();

    if (!chats.contains(name)) {
        logger.log(QtWarningMsg, QString("Чат '%1' не найден для удаления.").arg(name));
        return false;
    }

    if (!database.deleteChat(name)) {
        logger.log(QtWarningMsg, QString("Не удалось удалить чат '%1' из базы данных.").arg(name));
        return false;
    }

    chats.remove(name);

    for (int row = 0; row < model.rowCount(); ++row) {
        QStandardItem* item = model.item(row);
        if (item && item->text() == name) {
            model.removeRow(row);
            break;
        }
    }

    logger.log(QtInfoMsg, QString("Чат '%1' успешно удален.").arg(name));
    emit chatDeleted();
    return true;
}
