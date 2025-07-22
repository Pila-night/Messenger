#ifndef CHAT_H
#define CHAT_H

#include <QString>
#include <QList>
#include <QDateTime>
#include "Message.h"

/**
 * @brief Класс Chat представляет собой модель чата.
 *
 * Класс управляет списком сообщений в чате и предоставляет методы для добавления новых сообщений
 * Каждое сообщение представлено объектом класса Message,
 * который хранится в виде умного указателя (std::shared_ptr).
 */

class Chat {
private:
    QString name;
    QList<std::shared_ptr<Message>> messages; /*Список сообщений*/

public:
    Chat(const QString& chatName);
    void addMessage(const QString& sender, const QString& text, const QDateTime& timestamp);
    const QList<std::shared_ptr<Message>>& getMessages() const;
    QString getName() const;
    ~Chat() = default;
};
#endif // CHAT_H
