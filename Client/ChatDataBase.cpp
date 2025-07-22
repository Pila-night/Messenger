#include "chatdatabase.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QDateTime>

ChatDatabase::ChatDatabase(QObject* parent)
    : QObject(parent) {}

ChatDatabase::~ChatDatabase() {
    close();
}

/**
 * @brief open - открывает соединение с базой данных сообщений.
 * Создает таблицу messages, если она не существует.
 * @param path - путь к файлу базы данных.
 * @return true, если соединение успешно установлено, иначе false.
 */

bool ChatDatabase::open(const QString& path) {
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(path);

    if (!db.open()) {
        qWarning() << "Не удалось открыть базу данных:" << db.lastError().text();
        return false;
    }

    QSqlQuery query;
    if (!query.exec("CREATE TABLE IF NOT EXISTS messages ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                    "chat_name TEXT NOT NULL, "
                    "sender TEXT NOT NULL, "
                    "text TEXT NOT NULL, "
                    "timestamp DATETIME NOT NULL)")) {
        qWarning() << "Ошибка при создании таблицы:" << query.lastError().text();
        return false;
    }

    return true;
}

/**
 * @brief ChatDatabase::close - закрывает соединение с базой данных.
 */
void ChatDatabase::close() {
    if (db.isOpen()) {
        db.close();
    }
}

/**
 * @brief addMessage - добавляет новое сообщение в базу данных.
 * @param chatName - имя чата.
 * @param sender - отправитель сообщения.
 * @param text - текст сообщения.
 * @param timestamp - временная метка сообщения.
 */
void ChatDatabase::addMessage(const QString& chatName, const QString& sender, const QString& text, const QDateTime& timestamp) {
    QSqlQuery query;
    query.prepare("INSERT INTO messages (chat_name, sender, text, timestamp) VALUES (?, ?, ?, ?)");
    query.addBindValue(chatName);
    query.addBindValue(sender);
    query.addBindValue(text);
    query.addBindValue(timestamp.toString(Qt::ISODate));

    if (!query.exec()) {
        qWarning() << "Ошибка при добавлении сообщения:" << query.lastError().text();
    }
}

/**
 * @brief getMessages - получает все сообщения из указанного чата.
 * @param chatName - имя чата.
 * @return Список сообщений в виде QList<QMap<QString, QString>>,
 * где каждый QMap содержит ключи "sender", "text" и "timestamp".
 */
QList<QMap<QString, QString>> ChatDatabase::getMessages(const QString& chatName) {
    QList<QMap<QString, QString>> messages;

    QSqlQuery query;
    query.prepare("SELECT sender, text, timestamp FROM messages WHERE chat_name = ? ORDER BY timestamp ASC");
    query.addBindValue(chatName);

    if (!query.exec()) {
        qWarning() << "Ошибка при получении сообщений:" << query.lastError().text();
        return messages;
    }

    while (query.next()) {
        QMap<QString, QString> message;
        message["sender"] = query.value(0).toString();
        message["text"] = query.value(1).toString();
        message["timestamp"] = query.value(2).toString();
        messages.append(message);
    }

    return messages;
}
