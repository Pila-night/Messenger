#include "ManagerNetwork.h"
#include "logger.h"
#include <QDebug>

ManagerNetwork::ManagerNetwork(QObject* parent)
    : QObject(parent) {
    connect(&server, &QTcpServer::newConnection, this, &ManagerNetwork::onNewConnection);
}

ManagerNetwork::~ManagerNetwork() {
    if (server.isListening()) {
        server.close();
    }
}

/**
 * @brief Запускает сервер на указанном порту.
 * @param port Порт для прослушивания.
 */
void ManagerNetwork::startServer(quint16 port, const QHostAddress& address)
{
    Logger& logger = Logger::getInstance();
    if (server.isListening()) {
        logger.log(QtWarningMsg, "Сервер уже запущен");
        return;
    }

    if (!server.listen(address, port)) {
        logger.log(QtCriticalMsg, QString("Не удалось запустить сервер: %1").arg(server.errorString()));
        emit errorOccurred("Не удалось запустить сервер: " + server.errorString());
        return;
    }
}

/**
 * @brief Отправляет сообщение конкретному пользователю.
 * @param username Имя пользователя.
 * @param data Данные для отправки.
 */
void ManagerNetwork::sendMessageToUser(QTcpSocket* socket, const QByteArray& data) {
    Logger& logger = Logger::getInstance();
    if (socket->state() == QAbstractSocket::ConnectedState) {
        socket->write(data);
        logger.log(QtInfoMsg, "Сообщение отправлено пользователю");
    } else {
        logger.log(QtWarningMsg, "Ошибка: соединение с пользователем не установлено.");
    }
}


/**
 * @brief Рассылает сообщение всем подключенным клиентам.
 * @param data Данные для рассылки.
 */
void ManagerNetwork::broadcastMessage(const QByteArray& data) {
    Logger& logger = Logger::getInstance();
    for (auto* socket : server.findChildren<QTcpSocket*>()) {
        if (socket->state() == QAbstractSocket::ConnectedState) {
            socket->write(data);
            logger.log(QtInfoMsg, QString("Сообщение отправлено клиенту: %1:%2")
                                      .arg(socket->peerAddress().toString())
                                      .arg(socket->peerPort()));
        }
    }
}




/**
 * @brief Обрабатывает новое подключение клиента.
 */
void ManagerNetwork::onNewConnection() {
    Logger& logger = Logger::getInstance();
    QTcpSocket* socket = server.nextPendingConnection();

    connect(socket, &QTcpSocket::readyRead, this, &ManagerNetwork::onReadyRead);
    connect(socket, &QTcpSocket::disconnected, this, &ManagerNetwork::onClientDisconnected);
    connect(socket, &QTcpSocket::errorOccurred, this, &ManagerNetwork::onError);

    buffers[socket] = QByteArray();

    emit newConnection(socket);

    logger.log(QtInfoMsg, QString("Новое подключение: %1:%2") .arg(socket->peerAddress().toString())
                              .arg(socket->peerPort()));}

/**
 * @brief Обрабатывает входящие данные от клиентов.
 */
void ManagerNetwork::onReadyRead() {
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    Logger& logger = Logger::getInstance();
    if (!socket) {
        logger.log(QtWarningMsg, "Ошибка: не удалось определить сокет.");
        return;
    }

    QByteArray data = socket->readAll();

    if (!data.isEmpty()) {
        logger.log(QtInfoMsg, QString("Получен пакет от %1:%2, размер: %3 байт")
                                  .arg(socket->peerAddress().toString())
                                  .arg(socket->peerPort())
                                  .arg(data.size()));
        emit dataReceived(socket, data);
    }
}

/**
 * @brief Обрабатывает отключение клиента.
 */
void ManagerNetwork::onClientDisconnected() {
    Logger& logger = Logger::getInstance();
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) {
        logger.log(QtWarningMsg, "Ошибка: не удалось определить "
                                 "сокет отсоединяющегося клиента.");
        return;
    }

    buffers.remove(socket);

    emit clientDisconnected(socket);

    logger.log(QtInfoMsg, QString("Клиент отключился: %1:%2")
                              .arg(socket->peerAddress().toString())
                              .arg(socket->peerPort()));
}
/**
 * @brief Обрабатывает ошибки соединения.
 * @param socketError Тип ошибки.
 */
void ManagerNetwork::onError(QAbstractSocket::SocketError socketError) {
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    Logger& logger = Logger::getInstance();
    if (!socket) {
        logger.log(QtWarningMsg, "Ошибка: не удалось определить сокет.");
        return;
    }

    QString errorMessage;

    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        errorMessage = "Клиент закрыл соединение.";
        break;
    case QAbstractSocket::ConnectionRefusedError:
        errorMessage = "Соединение отклонено клиентом.";
        break;
    case QAbstractSocket::HostNotFoundError:
        errorMessage = "Клиент не найден.";
        break;
    default:
        errorMessage = "Ошибка: " + socket->errorString();
        break;
    }

    qWarning() << errorMessage;
    emit errorOccurred(errorMessage);
}
