#ifndef PACKETHANDLER_H
#define PACKETHANDLER_H

#include <QObject>
#include "protocol.h"

/**
 * @brief Базовый класс PacketHandler.
 * Предоставляет интерфейс для обработки различных типов пакетов.
 */
class PacketHandler {
public:
    virtual ~PacketHandler() = default;

    /**
     * @brief Обрабатывает пакет регистрации.
     * @param packet Пакет регистрации.
     */
    virtual void handle(PacketRegister& packet) = 0;

    /**
     * @brief Обрабатывает пакет сообщения.
     * @param packet Пакет сообщения.
     */
    virtual void handle(PacketMessage& packet) = 0;

    /**
     * @brief Обрабатывает пакет ответа сервера.
     * @param packet Пакет ответа сервера.
     */
    virtual void handle(PacketServerResponse& packet) = 0;

    /**
     * @brief Обрабатывает пакет списка чатов.
     * @param packet Пакет списка чатов.
     */
    virtual void handle(PacketChatList& packet) = 0;

    /**
     * @brief Обрабатывает пакет авторизации.
     * @param packet Пакет авторизации.
     */
    virtual void handle(PacketAuth& packet) = 0;

private:
    QString salt; /*Соль для авторизации*/
};

/**
 * @brief Класс PacketServerResponseHandler.
 * Обрабатывает пакеты ответа сервера (авторизация, регистрация).
 */
class PacketServerResponseHandler : public QObject, public PacketHandler {
    Q_OBJECT

public:
    explicit PacketServerResponseHandler(QObject* parent = nullptr);

    void handle(PacketAuth& packet) override;
    void handle(PacketRegister& packet) override;
    void handle(PacketMessage& packet) override;
    void handle(PacketChatList& packet) override;
    void handle(PacketServerResponse& packet) override;

signals:
    /**
     * @brief Сигнал отправляется при получении соли для авторизации.
     * @param salt Соль для авторизации.
     */
    void authSaltReceived(const QString& salt);

    /**
     * @brief Сигнал отправляется при неудачной авторизации.
     * @param message Сообщение об ошибке.
     */
    void authFailed(const QString& message);

    /**
     * @brief Сигнал отправляется при успешной авторизации.
     */
    void authSuccess();

    /**
     * @brief Сигнал отправляется при успешной регистрации.
     */
    void RegisterSuccess();

    /**
     * @brief Сигнал отправляется при неудачной регистрации.
     * @param message Сообщение об ошибке.
     */
    void RegisterFailed(const QString& message);
};

/**
 * @brief Класс PacketMessageHandler.
 * Обрабатывает пакеты сообщений.
 */
class PacketMessageHandler : public QObject, public PacketHandler {
    Q_OBJECT

public:
    explicit PacketMessageHandler(QObject* parent = nullptr);

    void handle(PacketAuth& packet) override;
    void handle(PacketRegister& packet) override;
    void handle(PacketServerResponse& packet) override;
    void handle(PacketChatList& packet) override;
    void handle(PacketMessage& packet) override;

signals:
    /**
     * @brief Сигнал отправляется при получении нового сообщения.
     * @param firstName Имя отправителя.
     * @param lastName Фамилия отправителя.
     * @param chatName Имя чата.
     * @param sender Отправитель.
     * @param text Текст сообщения.
     * @param timestamp Временная метка.
     */
    void messageReceived(const QString& firstName, const QString& lastName,
                         const QString& chatName, const QString& sender,
                         const QString& text, const QDateTime& timestamp);
};

/**
 * @brief Класс PacketChatListHandler.
 * Обрабатывает пакеты списка чатов.
 */
class PacketChatListHandler : public QObject, public PacketHandler {
    Q_OBJECT

public:
    explicit PacketChatListHandler(QObject* parent = nullptr);

    void handle(PacketAuth& packet) override;
    void handle(PacketRegister& packet) override;
    void handle(PacketMessage& packet) override;
    void handle(PacketServerResponse& packet) override;
    void handle(PacketChatList& packet) override;

signals:
    /**
     * @brief Сигнал отправляется при получении списка чатов.
     * @param chatList Список имен чатов.
     */
    void chatListReceived(const QStringList& chatList);
};

#endif // PACKETHANDLER_H
