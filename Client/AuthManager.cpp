#include "AuthManager.h"


AuthManager::AuthManager(ManagerNetwork *managerNetwork, QObject *parent)
    : QObject(parent), m_managerNetwork(managerNetwork)
{
    connect(m_managerNetwork, &ManagerNetwork::packetReceived,
            this, &AuthManager::handlePacketReceived);
}

QString AuthManager::hashPassword(const QString &password, const QString &salt){
    QByteArray str = (password + salt).toUtf8();
    QByteArray hash = QCryptographicHash::hash(str,QCryptographicHash::Sha256);
    return QString(hash.toHex());
}

void AuthManager::registerUser(const QString &username, const QString &password){
    Register reg;
    reg.setUsername(username);
    reg.setPassword(password);
    m_managerNetwork->sendPacket(std::make_unique<Register>(reg));

}

void AuthManager::authUser(const QString &username, const QString &password) {
    qDebug() << "Отправка первого этапа авторизации (только логин)";
    currentUsername = username;
    currentPassword = password;

    Auth auth;
    auth.setUsername(username);

    waitingForSalt = true;
    m_managerNetwork->sendPacket(std::make_unique<Auth>(auth));
}

void AuthManager::sendFinalAuthHash() {
    Auth auth;
    auth.setUsername(currentUsername);

    QString hashed = hashPassword(currentPassword, salt);
    auth.setPassword(hashed);

    m_managerNetwork->sendPacket(std::make_shared<Auth>(auth));
}
void AuthManager::handlePacketReceived(std::shared_ptr<Packet> packet) {
    if (!packet) {
        return;
    }

    if (packet->getType() == PacketType::ServerResponse) {
        auto response = std::dynamic_pointer_cast<ServerResponse>(packet);
        if (!response) return;

        QString message = response->getResponse();

        if (message == "ERR_AUTH") {
            emit authFail("ERR_AUTH");
            waitingForSalt = false;
            return;
        }
    }

    if (packet->getType() == PacketType::AuthChallenge) {
        auto authChallenge = std::dynamic_pointer_cast<AuthChallenge>(packet);
        if (authChallenge) {
            salt = authChallenge->getSalt();
            qDebug() << "Получена соль:" << salt;
            sendFinalAuthHash();
        }
    }
}


