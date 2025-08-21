#ifndef PACKETHANDLER_H
#define PACKETHANDLER_H

#include <QObject>
#include <QString>
#include <QTcpSocket>
#include <QMap>
#include "protocol.h"
#include "ClientDataBase.h"
#include "ManagerNetwork.h"
#include "ChatManager.h"


/**
 * @brief Базовый класс PacketHandler.
 * Предоставляет интерфейс для обработки различных типов пакетов.
 */
class PacketHandler {
public:
    virtual ~PacketHandler() = default;

    /**
     * @brief Обрабатывает пакет регистрации.
     * @param socket Сокет клиента.
     * @param packet Пакет регистрации.
     */
    virtual void handle(QTcpSocket* socket, PacketRegister& packet) = 0;

    /**
     * @brief Обрабатывает пакет сообщения.
     * @param socket Сокет клиента.
     * @param packet Пакет сообщения.
     */
    virtual void handle(QTcpSocket* socket, PacketMessage& packet) = 0;

    /**
     * @brief Обрабатывает пакет ответа сервера.
     * @param socket Сокет клиента.
     * @param packet Пакет ответа сервера.
     */
    virtual void handle(QTcpSocket* socket, PacketServerResponse& packet) = 0;

    /**
     * @brief Обрабатывает пакет списка чатов.
     * @param socket Сокет клиента.
     * @param packet Пакет списка чатов.
     */
    virtual void handle(QTcpSocket* socket, PacketChatList& packet) = 0;

    /**
     * @brief Обрабатывает пакет авторизации.
     * @param socket Сокет клиента.
     * @param packet Пакет авторизации.
     */
    virtual void handle(QTcpSocket* socket, PacketAuth& packet) = 0;

protected:
    QString salt; ///< Соль для авторизации.
};

/**
 * @brief Класс PacketRegisterHandler.
 * Обрабатывает пакеты регистрации.
 */
class PacketRegisterHandler : public QObject, public PacketHandler {
    Q_OBJECT

private:
    ClientDataBase* clientDataBase;
    ManagerNetwork* managerNetwork;

public:
    /**
     * @brief Конструктор класса PacketRegisterHandler.
     * @param db Указатель на базу данных клиентов.
     * @param managerNetwork Указатель на менеджер сети.
     * @param parent Родительский объект.
     */
    PacketRegisterHandler(ClientDataBase* db, ManagerNetwork* managerNetwork, QObject* parent = nullptr);

    void handle(QTcpSocket* socket, PacketAuth& packet) override {}
    void handle(QTcpSocket* socket, PacketRegister& packet) override;
    void handle(QTcpSocket* socket, PacketMessage& packet) override {}
    void handle(QTcpSocket* socket, PacketChatList& packet) override {}
    void handle(QTcpSocket* socket, PacketServerResponse& packet) override {}

signals:
    void registrationSuccess(const QString& username); ///< Сигнал успешной регистрации.
    void registrationFailed(const QString& message);   ///< Сигнал неудачной регистрации.
};

/**
 * @brief Класс PacketAuthHandler.
 * Обрабатывает пакеты авторизации.
 */
class PacketAuthHandler : public QObject, public PacketHandler {
    Q_OBJECT

private:
    ClientDataBase* clientDataBase;
    ManagerNetwork* managerNetwork;

    struct AuthState {
        QString username;
        bool hasSentSalt;
    };
    QMap<QTcpSocket*, AuthState> socketStates;

public:
    /**
     * @brief Конструктор класса PacketAuthHandler.
     * @param db Указатель на базу данных клиентов.
     * @param managerNetwork Указатель на менеджер сети.
     * @param parent Родительский объект.
     */
    PacketAuthHandler(ClientDataBase* db, ManagerNetwork* managerNetwork, QObject* parent = nullptr);

    void handle(QTcpSocket* socket, PacketAuth& packet) override;
    void handle(QTcpSocket* socket, PacketRegister& packet) override {}
    void handle(QTcpSocket* socket, PacketMessage& packet) override {}
    void handle(QTcpSocket* socket, PacketChatList& packet) override {}
    void handle(QTcpSocket* socket, PacketServerResponse& packet) override {}

signals:
    void authSuccess();                     ///< Сигнал успешной авторизации.
    void authFailed(const QString& message); ///< Сигнал неудачной авторизации.
};

/**
 * @brief Класс PacketMessageHandler.
 * Обрабатывает пакеты сообщений.
 */
class PacketMessageHandler : public QObject, public PacketHandler {
    Q_OBJECT

private:
    ClientDataBase* clientDataBase;
    ManagerNetwork* managerNetwork;
    ChatManager* chatManager;

public:
    /**
     * @brief Конструктор класса PacketMessageHandler.
     * @param db Указатель на базу данных клиентов.
     * @param managerNetwork Указатель на менеджер сети.
     * @param chatManager Указатель на менеджер чатов.
     * @param parent Родительский объект.
     */
    PacketMessageHandler(ClientDataBase* db, ManagerNetwork* managerNetwork, ChatManager* chatManager, QObject* parent = nullptr);

    void handle(QTcpSocket* socket, PacketAuth& packet) override {}
    void handle(QTcpSocket* socket, PacketRegister& packet) override {}
    void handle(QTcpSocket* socket, PacketMessage& packet) override;
    void handle(QTcpSocket* socket, PacketChatList& packet) override {}
    void handle(QTcpSocket* socket, PacketServerResponse& packet) override {}

signals:
    void messageReceived(const QString& firstName, const QString& lastName,
                         const QString& chatName, const QString& sender,
                         const QString& text, const QDateTime& timestamp); ///< Сигнал получения нового сообщения.
};

/**
 * @brief Класс PacketChatListHandler.
 * Обрабатывает пакеты списка чатов.
 */
class PacketChatListHandler : public QObject, public PacketHandler {
    Q_OBJECT

private:
    ChatManager* chatManager;
    ManagerNetwork* managerNetwork;

public:
    /**
     * @brief Конструктор класса PacketChatListHandler.
     * @param manager Указатель на менеджер чатов.
     * @param managerNetwork Указатель на менеджер сети.
     * @param parent Родительский объект.
     */
    PacketChatListHandler(ChatManager* manager, ManagerNetwork* managerNetwork, QObject* parent = nullptr);

    void handle(QTcpSocket* socket, PacketAuth& packet) override {}
    void handle(QTcpSocket* socket, PacketRegister& packet) override {}
    void handle(QTcpSocket* socket, PacketMessage& packet) override {}
    void handle(QTcpSocket* socket, PacketServerResponse& packet) override {}
    void handle(QTcpSocket* socket, PacketChatList& packet) override;

signals:
    void chatListReceived(const QStringList& chatList); ///< Сигнал получения списка чатов.
};
#endif // PACKETHANDLER_H
