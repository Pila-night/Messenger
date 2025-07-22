#include "AuthManager.h"


AuthManager::AuthManager(QObject *parent)
    : QObject(parent)
{

}


/**
 * @brief Хэширует строку вида  password+salt по алгоритму SHA-256.
 *
 * Возвращается шестнадцатеричное представление хэша в виде QByteArray.
 *
 * @param password Пароль пользователя в виде строки.
 * @param salt Соль в виде строки
 * @return QString Хэш пароля в формате HEX
 */
QString AuthManager::hashPassword(const QString &password, const QString &salt){
    QByteArray str = (password + salt).toUtf8();
    QByteArray hash = QCryptographicHash::hash(str,QCryptographicHash::Sha256);
    return QString(hash.toHex());
}
QString AuthManager::getHashPassword(const QString &password, const QString &salt)
{
    return hashPassword(password, salt);
}

