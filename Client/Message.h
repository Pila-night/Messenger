#ifndef MESSAGE_H
#define MESSAGE_H
#include <QString>
#include <QDateTime>

/**
 * @brief Класс Message представляет собой модель сообщения.
 *
 * клсс хранит информацию о сообщении, включая отправителя,
 * текст сообщения и временную метку.
 */

class Message {
private:
    QString sender; /*Отправитель сообщения*/
    QString text;   /* Текст сообщения*/
    QDateTime timestamp; /* Временная метка сообщения*/

public:
    Message(const QString& sender, const QString& text, const QDateTime& timestamp);
    QString getSender() const;
    void setSender(const QString& s);
    QString getText() const;
    void setText(const QString& t);
    QDateTime getTimestamp() const;
    void setTimestamp(const QDateTime& ts);
};

#endif // MESSAGE_H
