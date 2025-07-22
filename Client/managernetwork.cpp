#include "managernetwork.h"

/*Выполнить потом проверку*/
ManagerNetwork::ManagerNetwork(QObject *parent)
    : QObject(parent) {
    connect(&socket, &QTcpSocket::readyRead, this, &ManagerNetwork::packetRead);
    connect(&socket, &QTcpSocket::connected, this, &ManagerNetwork::onConnected);
    connect(&socket, &QTcpSocket::errorOccurred, this, &ManagerNetwork::onError);
}

ManagerNetwork::~ManagerNetwork() = default;

/**
 * @brief connectToServer - подключается к серверу.
 * @param server Адрес сервера.
 * @param port Порт сервера.
 */
void ManagerNetwork::connectToServer(const QString &server, qint16 port){

    socket.connectToHost(server, port);
}

/**
 * @brief sendPacket - отправляет пакет данных на сервер.
 * @param data Данные для отправки.
 */
void ManagerNetwork::sendPacket(QByteArray data){
    socket.write(data);
}

/**
 * @brief packetRead - обрабатывает входящие данные от сервера.
 * Эмитирует сигнал dataReceived, если данные не пусты.
 */
void ManagerNetwork::packetRead()
{
    QByteArray data = socket.readAll();
    if (!data.isEmpty()) {
        emit dataReceived(data);
    }
}


void ManagerNetwork::onConnected(){
    emit connected();
}

/*Переосмыслить и доделать*/
/**
 * @brief onError - обрабатывает ошибки соединения.
 * @param socketError Тип ошибки.
 */
void ManagerNetwork::onError(QAbstractSocket::SocketError socketError){
    QString errorMessage;
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        errorMessage = "Сервер закрыл соединение.";
        break;
    case QAbstractSocket::ConnectionRefusedError:
        errorMessage = "Соединение отклонено сервером.";
        break;
    case QAbstractSocket::HostNotFoundError:
        errorMessage = "Сервер не найден.";
        break;
    default:
        errorMessage = "Ошибка: " + socket.errorString();
        break;
    }
    emit error(errorMessage);
}




