#pragma once

#include <QObject>
#include "managernetwork.h"
#include "protocol.h"
#include <QCryptographicHash>

class AuthManager : public QObject
{
    Q_OBJECT
public:
    explicit AuthManager(ManagerNetwork *managerNetwork, QObject *parent = nullptr);
    void registerUser(const QString &username, const QString &password);
    void authUser(const QString &username, const QString &password);

signals:
    void okRegisterUser();
    void okAuthUser();
    void authFail(const QString &mes);

private slots:
    void handlePacketReceived(std::shared_ptr<Packet> packet);

private:
    ManagerNetwork *m_managerNetwork;
    QString currentUsername;
    QString currentPassword;
    QString salt;
    bool waitingForSalt = false;

    /*void handleRegistrationResponse(const ServerResponse &response);
    void handleAuthResponse(const ServerResponse &response);*/
    void sendFinalAuthHash();


    QByteArray hashPassword(const QString &password, const QString &salt);
};


