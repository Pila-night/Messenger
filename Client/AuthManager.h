#pragma once

#include <QObject>
#include <QCryptographicHash>



/*
 *
 *
 * Переделать
 *
 *
 */


class AuthManager : public QObject
{
    Q_OBJECT
public:
    AuthManager(QObject *parent = nullptr);
   /* void registerUser(const QString &username, const QString &password);
    void authUser(const QString &username, const QString &password);*/
    QString getHashPassword(const QString &password, const QString &salt);
signals:
    void okRegisterUser();
    void okAuthUser();
    void authFail(const QString &mes);

private slots:
    /*void handlePacketReceived(std::shared_ptr<Packet> packet);*/

private:
  /*  QString currentUsername;
    QString currentPassword;
    QString salt;*/
    //bool waitingForSalt = false;

    /*void handleRegistrationResponse(const ServerResponse &response);
    void handleAuthResponse(const ServerResponse &response);*/
    //void sendFinalAuthHash();

    QString hashPassword(const QString &password, const QString &salt);
};


