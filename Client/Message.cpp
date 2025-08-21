#include "Message.h"

/**
 * @brief Конструктор класса Message.
 * @param sender Отправитель сообщения.
 * @param text Текст сообщения.
 * @param timestamp Временная метка сообщения.
 * @param firstName Имя отправителя.
 * @param lastName Фамилия отправителя.
 */
Message::Message(const QString& sender, const QString& text, const QDateTime& timestamp,
                 const QString& firstName, const QString& lastName)
    : firstName(firstName), lastName(lastName), sender(sender), text(text), timestamp(timestamp) {}

/**
 * @brief Копирующий конструктор.
 * Создает копию существующего объекта Message.
 * @param other Объект, который нужно скопировать.
 */
Message::Message(const Message& other)
    : firstName(other.firstName), lastName(other.lastName)
    , sender(other.sender) , text(other.text)
    , timestamp(other.timestamp)
{}

/**
 * @brief Перемещающий конструктор.
 * Создает новый объект, забирая данные из другого объекта.
 * @param other Объект, данные которого будут перемещены.
 */
Message::Message(Message&& other) noexcept :
    firstName(std::move(other.firstName))
    , lastName(std::move(other.lastName))
    , sender(std::move(other.sender))
    , text(std::move(other.text))
    , timestamp(std::move(other.timestamp))
{}

/**
 * @brief Оператор копирующего присваивания.
 * Копирует данные из одного объекта в другой.
 * @param other Объект, который нужно скопировать.
 * @return Ссылка на текущий объект.
 */
Message& Message::operator=(const Message& other) {
    if (this != &other) {
        firstName = other.firstName;
        lastName = other.lastName;
        sender = other.sender;
        text = other.text;
        timestamp = other.timestamp;
    }
    return *this;
}
/**
 * @brief Оператор перемещающего присваивания.
 * Перемещает данные из одного объекта в другой.
 * @param other Объект, данные которого будут перемещены.
 * @return Ссылка на текущий объект.
 */
Message& Message::operator=(Message&& other) noexcept {
    if (this != &other) {
        firstName = std::move(other.firstName);
        lastName = std::move(other.lastName);
        sender = std::move(other.sender);
        text = std::move(other.text);
        timestamp = std::move(other.timestamp);
    }
    return *this;
}

/**
 * @brief Деструктор.
 * =). Ну как бы, освобождать нечего, но надо
 */
Message::~Message() {}

/**
 * @brief getFirstName - возвращает имя отправителя.
 * @return Имя отправителя в виде QString.
 */
QString Message::getFirstName() const {
    return firstName;
}

/**
 * @brief setFirstName - устанавливает имя отправителя.
 * @param fn Новое имя отправителя.
 */
void Message::setFirstName(const QString& fn) {
    firstName = fn;
}

/**
 * @brief getLastName - возвращает фамилию отправителя.
 * @return Фамилия отправителя в виде QString.
 */
QString Message::getLastName() const {
    return lastName;
}

/**
 * @brief setLastName - устанавливает фамилию отправителя.
 * @param ln Новая фамилия отправителя.
 */
void Message::setLastName(const QString& ln) {
    lastName = ln;
}

/**
 * @brief getSender - возвращает имя отправителя сообщения.
 * @return Имя отправителя в виде QString.
 */
QString Message::getSender() const {
    return sender;
}

/**
 * @brief setSender - устанавливает отправителя сообщения.
 * @param s Имя отправителя.
 */
void Message::setSender(const QString& s) {
    sender = s;
}

/**
 * @brief getText - возвращает текст сообщения.
 * @return Текст сообщения в виде QString.
 */
QString Message::getText() const {
    return text;
}

/**
 * @brief setText - устанавливает текст сообщения.
 * @param t Текст сообщения.
 */
void Message::setText(const QString& t) {
    text = t;
}

/**
 * @brief getTimestamp - возвращает временную метку сообщения.
 * @return Временная метка в виде QDateTime.
 */
QDateTime Message::getTimestamp() const {
    return timestamp;
}

/**
 * @brief setTimestamp - устанавливает временную метку сообщения.
 * @param ts Новая временная метка.
 */
void Message::setTimestamp(const QDateTime& ts) {
    timestamp = ts;
}

