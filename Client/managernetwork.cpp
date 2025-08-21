#include "managernetwork.h"
#include "logger.h"


ManagerNetwork::ManagerNetwork(QObject *parent)
    : QObject(parent) {
    connect(&socket, &QTcpSocket::readyRead, this, &ManagerNetwork::packetRead);
    connect(&socket, &QTcpSocket::connected, this, &ManagerNetwork::onConnected);

    connect(&socket, &QTcpSocket::errorOccurred, this, [this](QAbstractSocket::SocketError error) {
        Logger& logger = Logger::getInstance();
        logger.log(QtCriticalMsg, QString("Ошибка сокета: ") + socket.errorString());
    });
}

ManagerNetwork::~ManagerNetwork() = default;

/**
 * @brief connectToServer - подключается к серверу.
 * @param server Адрес сервера.
 * @param port Порт сервера.
 */
void ManagerNetwork::connectToServer(const QString &server, qint16 port) {
    Logger& logger = Logger::getInstance();

    if (socket.state() == QAbstractSocket::UnconnectedState) {
        logger.log(QtInfoMsg, QString("Подключение к серверу - %1:%2").arg(server).arg(port));
        socket.connectToHost(server, port);
    } else {
        logger.log(QtWarningMsg, "Сокет уже находится в процессе подключения или подключен.");
    }
}

/**
 * @brief sendPacket - отправляет пакет данных на сервер.
 * @param data Данные для отправки.
 */
void ManagerNetwork::sendPacket(QByteArray data){
    Logger& logger = Logger::getInstance();
    logger.log(QtInfoMsg, QString("Отправка данных на сервер. Размер данных: %1 байт").arg(data.size()));
    socket.write(data);
}

/**
 * @brief packetRead - обрабатывает входящие данные от сервера.
 * Эмитирует сигнал dataReceived, если данные не пусты.
 */
void ManagerNetwork::packetRead()
{
    QByteArray data = socket.readAll();
    Logger& logger = Logger::getInstance();
    logger.log(QtInfoMsg, QString("Получен пакет данных с сервера. Размер данных: %1 байт").arg(data.size()));
    if (!data.isEmpty()) {
        emit dataReceived(data);
    }
}


void ManagerNetwork::onConnected(){
    emit connected();
}
/**
 * @brief Отключениее от сервера.
 */
void ManagerNetwork::disconnectFromServer() {
    Logger& logger = Logger::getInstance();

    if (socket.state() != QAbstractSocket::UnconnectedState) {
        logger.log(QtInfoMsg, "Отключение от сервера");
        socket.disconnectFromHost();

    }
}

/**
 * @brief Проверяет, подключен ли сокет к серверу.
 * @return true, если сокет подключен, иначе false.
 */
bool ManagerNetwork::isConnected() {
    return socket.state() == QAbstractSocket::ConnectedState;
}
