#ifndef CHAT_H
#define CHAT_H

#include <QString>
#include <QList>
#include <QDateTime>
#include "Message.h"

/**
 * @brief Класс Chat представляет собой модель чата.
 *
 * Класс управляет списком сообщений и предоставляет методы для работы с ними.
 */

class Chat {
private:
    QString name;          /*Имя чата*/
    QList<Message> messages; /* Список сообщений в чате*/

public:
    /**
     * @brief Конструктор класса Chat.
     * @param chatName Имя чата.
     */
    Chat(const QString& chatName);

    void addMessage(const QString& sender, const QString& text, const QDateTime& timestamp,
                    const QString& firstName, const QString& lastName);

    const QList<Message>& getMessages() const;

    QString getName() const;

    ~Chat() = default;
};

#endif // CHAT_H
