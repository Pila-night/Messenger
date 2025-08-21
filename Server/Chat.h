#ifndef CHAT_H
#define CHAT_H

#include <QString>
#include <QList>
#include <QDateTime>
#include "Message.h"

/**
 * @brief Класс Chat представляет собой модель чата.
 *
 * Класс управляет списком сообщений в чате и предоставляет методы для работы с ними.
 * Каждое сообщение представлено объектом класса Message.
 */

class Chat {
private:
    QString name;
    QList<Message> messages;

public:
    Chat();
    Chat(const QString& chatName);
    void addMessage(const QString& sender, const QString& text, const QDateTime& timestamp,
                    const QString& firstName, const QString& lastName);
    const QList<Message>& getMessages() const;
    QString getName() const;
    ~Chat() = default;
};
#endif // CHAT_H
