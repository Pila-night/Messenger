#include "AuthManager.h"


AuthManager::AuthManager(ManagerNetwork *managerNetwork, QObject *parent)
    : QObject(parent), m_managerNetwork(managerNetwork)
{
    connect(m_managerNetwork, &ManagerNetwork::packetReceived,
            this, &AuthManager::handlePacketReceived);
}


/**
 * @brief Хэширует строку вида  password+salt по алгоритму SHA-256.
 *
 * Возвращается шестнадцатеричное представление хэша в виде QByteArray.
 *
 * @param password Пароль пользователя в виде строки.
 * @param salt Соль в виде строки
 * @return QByteArray Хэш пароля в формате HEX
 */
QByteArray AuthManager::hashPassword(const QString &password, const QString &salt){
    QByteArray str = (password + salt).toUtf8();
    QByteArray hash = QCryptographicHash::hash(str,QCryptographicHash::Sha256);
    return hash.toHex();
}


void AuthManager::handlePacketReceived(std::shared_ptr<Packet> packet) {
    if (!packet) {
        return;
    }

    if (packet->getType() == PacketType::ServerResponse) {
        auto response = std::dynamic_pointer_cast<PacketServerResponse>(packet);
        if (!response) return;

        QString message = response->getResponse();

        if (message == "ERR_AUTH") {
            emit authFail("ERR_AUTH");
            waitingForSalt = false;
            return;
        }
    }

    if (packet->getType() == PacketType::AuthChallenge) {
        auto authChallenge = std::dynamic_pointer_cast<PacketAuthChallenge>(packet);
        if (authChallenge) {
            salt = authChallenge->getSalt();
            qDebug() << "Получена соль:" << salt;
            sendFinalAuthHash();
        }
    }
}


