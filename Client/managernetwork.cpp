#include "managernetwork.h"
ManagerNetwork::ManagerNetwork(QObject *parent)
    : QObject(parent) {
    connect(&socket, &QTcpSocket::readyRead, this, &ManagerNetwork::onReadyRead);
    connect(&socket, &QTcpSocket::connected, this, &ManagerNetwork::onConnected);
    connect(&socket, &QTcpSocket::errorOccurred, this, &ManagerNetwork::onError);
}

ManagerNetwork::~ManagerNetwork() = default;

void ManagerNetwork::connectToServer(const QString &server, qint16 port){

    socket.connectToHost(server, port);
}

void ManagerNetwork::sendPacket(std::shared_ptr<Packet> packet){
    QByteArray data = packet -> serialize(); //сериализую данные
    socket.write(data);// отправляю данные

    /* не забыть реализовать проверку отправки!!*/
}

void ManagerNetwork::onReadyRead() {
    buf.append(socket.readAll());
    while(1){
        ByteBuffer byteBuffer(buf);

        if (byteBuffer.getAvailableBytes() < sizeof(qint32)) {
            break;
        }

        qint32 packetLength = byteBuffer.readIntLE();

        if (byteBuffer.getAvailableBytes() < packetLength){
            qDebug() << "Не все данные пришли";
            break;
        }

        QByteArray packetData = byteBuffer.read(packetLength);

        buf.remove(0, sizeof(qint32) + packetLength); //очищаю буфер

        auto packet = Packet::deserialize(packetData);
        if( packet ) {
            emit packetReceived(std::move(packet));
        }

    }
}


void ManagerNetwork::onConnected(){
    emit connected();
}

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




