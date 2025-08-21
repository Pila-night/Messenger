#include "chatdatabase.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QDateTime>
#include "logger.h"

/**
 * @brief Конструктор класса ChatDatabase.
 * @param parent.
 */
ChatDatabase::ChatDatabase(QObject* parent)
    : QObject(parent) {}

/**
 * @brief Деструктор класса ChatDatabase.
 * Закрывает соединение с базой данных при уничтожении объекта.
 */
ChatDatabase::~ChatDatabase() {
    close();
}

/**
 * @brief Открывает соединение с базой данных сообщений.
 * Создает таблицу messages, если она не существует.
 * @param path Путь к файлу базы данных.
 * @return true, если соединение успешно установлено, иначе false.
 */
bool ChatDatabase::open(const QString& path) {
    Logger& logger = Logger::getInstance();
    logger.log(QtInfoMsg, QString("Попытка открыть базу данных по пути: %1").arg(path));

    db = QSqlDatabase::addDatabase("QSQLITE", "ChatDatabase");
    db.setDatabaseName(path);

    if (!db.open()) {
        logger.log(QtCriticalMsg, QString("Не удалось открыть базу данных: %1").arg(db.lastError().text()));
        return false;
    }

    QSqlQuery query(db);
    if (!query.exec("CREATE TABLE IF NOT EXISTS messages ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                    "chat_name TEXT NOT NULL, "
                    "sender TEXT NOT NULL, "
                    "firstName TEXT NOT NULL, "
                    "lastName TEXT NOT NULL, "
                    "text TEXT NOT NULL, "
                    "timestamp DATETIME NOT NULL)")) {
        logger.log(QtWarningMsg, QString("Ошибка при создании таблицы: %1").arg(query.lastError().text()));
        return false;
    }
    logger.log(QtInfoMsg, "Таблица messages успешно создана или уже существует.");
    return true;
}

/**
 * @brief Закрывает соединение с базой данных.
 */
void ChatDatabase::close() {
    if (db.isOpen()) {
        db.close();
        qDebug() << "Соединение с базой данных закрыто.";
    }
}

/**
 * @brief Добавляет новое сообщение в базу данных.
 * @param chatName Имя чата.
 * @param sender Отправитель сообщения.
 * @param text Текст сообщения.
 * @param timestamp Временная метка сообщения.
 * @param firstName Имя отправителя.
 * @param lastName Фамилия отправителя.
 */
void ChatDatabase::addMessage(const QString& chatName, const QString& sender, const QString& text,
                              const QDateTime& timestamp, const QString& firstName, const QString& lastName) {
    QSqlQuery query(db);
    query.prepare("INSERT INTO messages (chat_name, sender, text, timestamp, firstName, lastName) "
                  "VALUES (:chat_name, :sender, :text, :timestamp, :firstName, :lastName)");
    query.bindValue(":chat_name", chatName);
    query.bindValue(":sender", sender);
    query.bindValue(":text", text);
    query.bindValue(":timestamp", timestamp.toString(Qt::ISODate));
    query.bindValue(":firstName", firstName);
    query.bindValue(":lastName", lastName);

    Logger& logger = Logger::getInstance();
    if (!query.exec()) {
        logger.log(QtWarningMsg, QString("Ошибка при добавлении сообщения: %1").arg(query.lastError().text()));
    } else {
        logger.log(QtInfoMsg, "Сообщение успешно добавлено в базу данных.");
    }
}

/**
 * @brief Получает все сообщения из указанного чата.
 * @param chatName Имя чата.
 * @return Список сообщений в виде QList<QMap<QString, QString>>,
 * где каждый QMap содержит ключи "sender", "text" и "timestamp".
 */
QList<QMap<QString, QString>> ChatDatabase::getMessages(const QString& chatName) {
    QList<QMap<QString, QString>> messages;

    QSqlQuery query(db);
    query.prepare("SELECT sender, text, timestamp FROM messages WHERE chat_name = ? ORDER BY timestamp ASC");
    query.addBindValue(chatName);

    Logger& logger = Logger::getInstance();
    if (!query.exec()) {
        logger.log(QtWarningMsg, QString("Ошибка при получении сообщений из чата '%1': %2")
                                     .arg(chatName, query.lastError().text()));
        return messages;
    }

    while (query.next()) {
        QMap<QString, QString> message;
        message["sender"] = query.value(0).toString();
        message["text"] = query.value(1).toString();
        message["timestamp"] = query.value(2).toString();
        messages.append(message);
    }

    logger.log(QtInfoMsg, QString("Получено %1 сообщений для чата '%2'")
                              .arg(messages.size()).arg(chatName));
    return messages;
}

/**
 * @brief Проверяет, открыто ли соединение с базой данных.
 * @return true, если соединение открыто, иначе false.
 */
bool ChatDatabase::isOpen() const {
    return db.isOpen();
}

/**
 * @brief Получает список всех уникальных имен чатов из базы данных.
 * @return Список имен чатов.
 */
QStringList ChatDatabase::getAllChatNames() {
    QStringList chatNames;
    Logger& logger = Logger::getInstance();

    if (!db.isOpen()) {
        logger.log(QtWarningMsg, "База данных не открыта!");
        return chatNames;
    }

    QSqlQuery query(db);
    if (!query.exec("SELECT DISTINCT chat_name FROM messages")) {
        logger.log(QtWarningMsg, QString("Ошибка при получении списка чатов: %1").arg(query.lastError().text()));
        return chatNames;
    }

    qDebug() << "Запрос выполнен. Найдено чатов:" << query.size();

    while (query.next()) {
        QString chatName = query.value(0).toString();
        chatNames.append(chatName);
    }

    return chatNames;
}

/**
 * @brief Добавляет новый чат в базу данных.
 * Если чат уже существует, он не будет добавлен повторно.
 * @param chatName Имя чата.
 */
void ChatDatabase::addChat(const QString& chatName) {
    QSqlQuery query(db);
    query.prepare("INSERT OR IGNORE INTO messages (chat_name, sender, text, timestamp, firstName, lastName) "
                  "VALUES (?, ?, ?, ?, ?, ?)");
    query.addBindValue(chatName);
    query.addBindValue("System");
    query.addBindValue("Chat created");
    query.addBindValue(QDateTime::currentDateTime());
    query.addBindValue("");
    query.addBindValue("");

    Logger& logger = Logger::getInstance();

    if (!query.exec()) {
        logger.log(QtWarningMsg, QString("Ошибка при добавлении чата '%1': %2")
                                     .arg(chatName, query.lastError().text()));
    } else {
        logger.log(QtInfoMsg, QString("Чат '%1' успешно добавлен в базу данных.").arg(chatName));
    }
}

