#ifndef MANAGERNETWORK_H
#define MANAGERNETWORK_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QMap>
#include <QByteArray>

class ManagerNetwork : public QObject {
    Q_OBJECT

public:
    explicit ManagerNetwork(QObject* parent = nullptr);
    ~ManagerNetwork();

    void startServer(quint16 port, const QHostAddress& address = QHostAddress::Any); /* Запуск сервера на указанном порту*/
    void sendMessageToUser(QTcpSocket* socket, const QByteArray& data); /* Отправка сообщения конкретному пользователю*/
    void broadcastMessage(const QByteArray& data); /* Рассылка данных всем клиентам*/
    void associateUserWithSocket(const QString& username, QTcpSocket* socket); /* Связывание имени пользователя с сокетом*/

signals:
    void newConnection(QTcpSocket* socket); /* Сигнал о новом подключении*/
    void dataReceived(QTcpSocket* socket, const QByteArray& data); /* Сигнал о получении данных*/
    void clientDisconnected(QTcpSocket* socket); /* Сигнал об отключении клиента*/
    void errorOccurred(const QString& message); /* Сигнал об ошибке*/

private slots:
    void onNewConnection(); /* Обработка нового подключения*/
    void onReadyRead(); /* Обработка входящих данных*/
    void onClientDisconnected(); /* Обработка отключения клиента*/
    void onError(QAbstractSocket::SocketError socketError); /* Обработка ошибок*/

private:
    QTcpServer server; /* Сервер*/
    QMap<QTcpSocket*, QByteArray> buffers; /* Буферы для хранения данных от клиентов*/
};

#endif // MANAGERNETWORK_H
