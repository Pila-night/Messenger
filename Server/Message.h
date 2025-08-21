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
    QString firstName;   /*Имя отправителя*/
    QString lastName;    /*Фамилия отправителя*/
    QString sender;      /*Отправитель сообщения*/
    QString text;        /*Текст сообщения*/
    QDateTime timestamp; /*Временная метка */

public:
    // Конструктор
    Message(const QString& sender, const QString& text, const QDateTime& timestamp,
            const QString& firstName, const QString& lastName );


    Message(const Message& other);

    Message(Message&& other) noexcept;

    ~Message();

    Message& operator=(const Message& other);

    Message& operator=(Message&& other) noexcept;


    QString getFirstName() const;
    void setFirstName(const QString& fn);

    QString getLastName() const;
    void setLastName(const QString& ln);

    QString getSender() const;
    void setSender(const QString& s);

    QString getText() const;
    void setText(const QString& t);

    QDateTime getTimestamp() const;
    void setTimestamp(const QDateTime& ts);
};

#endif // MESSAGE_H
