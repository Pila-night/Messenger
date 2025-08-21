#pragma once
#include <QString>

namespace SecurityUtils {
    QString hashPassword(const QString& password, const QString& salt);
    QString generateSalt(int length = 16);
}
