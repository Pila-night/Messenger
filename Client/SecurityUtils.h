#pragma once
#include <QString>
/**
 * @brief Пространство имен SecurityUtils.
 * Содержит утилиты для работы с безопасностью, такие как генерация соли и хэширование паролей.
 */
namespace SecurityUtils {
QString hashPassword(const QString& password, const QString& salt);
QString generateSalt(int length = 16);
}
