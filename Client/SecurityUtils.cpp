#include "SecurityUtils.h"
#include <QRandomGenerator>
#include <QCryptographicHash>

namespace SecurityUtils {

/**
 * @brief Генерирует случайную строку (соль) заданной длины.
 * @param length Длина соли.
 * @return Сгенерированная соль.
*/
QString generateSalt(int length) {
    const QString possibleCharacters("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");

    QString salt;

    for(int i = 0; i < length; ++i) {
        int index = QRandomGenerator::global()->bounded(possibleCharacters.length());
        salt.append(possibleCharacters.at(index));
    }

    return salt;
}
/**
 * @brief Хэширует пароль с использованием соли.
 * @param password Пароль для хэширования.
 * @param salt Соль.
 * @return Хэшированный пароль в виде строки.
 */
QString hashPassword(const QString& password, const QString& salt) {
    QString passSalt = password + salt;
    QByteArray hash = QCryptographicHash::hash(
        passSalt.toUtf8(),
        QCryptographicHash::Sha256
        );
    return QString::fromUtf8(hash.toHex());
}
}
