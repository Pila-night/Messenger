#ifndef CHATDATABASE_H
#define CHATDATABASE_H

#include <QObject>
#include <QSqlDatabase>
#include <QString>

/**
 * @brief ChatDatabase - класс для работы с базой данных чатов.
 *
 * предоставляет методы для открытия базы данных, добавления сообщений
 * и получения сообщений из базы данных SQLite.
 */


class ChatDatabase : public QObject {
    Q_OBJECT

private:
    QSqlDatabase db;

public:
    ChatDatabase(QObject* parent = nullptr);
    ~ChatDatabase();

    bool open(const QString& path);
    bool isOpen() const;
    void close();
    QStringList getAllChatNames();

    void addChat(const QString& chatName);
    bool deleteChat(const QString& chatName);

    void addMessage(const QString& chatName, const QString& sender, const QString& text,
                    const QDateTime& timestamp, const QString& firstName, const QString& lastName);
    QList<QMap<QString, QString>> getMessages(const QString& chatName);  // Получить все сообщения чата
};

#endif
