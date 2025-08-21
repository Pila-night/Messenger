#include "ClientDataBase.h"
#include <QSqlQuery>
#include <QSqlError>
#include "logger.h"


/**
 * @brief Конструктор ClientDataBase.
 * Инициализирует подключение к базе данных SQLite и создает таблицу Users,
 * если она не существует.
 * @param path Путь к файлу базы данных.
 * @param parent.
 */
ClientDataBase::ClientDataBase(const QString& path, QObject* parent)
    : QObject(parent), db(QSqlDatabase::addDatabase("QSQLITE", "ClientDataBase" )) {

    db.setDatabaseName(path);
    Logger& logger = Logger::getInstance();
    if (!db.open()) {
        logger.log(QtCriticalMsg, QString("Ошибка открытия базы данных: %1").arg(db.lastError().text()));
    }

    QSqlQuery query(db);;
    bool success = query.exec("CREATE TABLE IF NOT EXISTS Users ("
                              "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                              "username TEXT UNIQUE NOT NULL, "
                              "first_name TEXT NOT NULL, "
                              "last_name TEXT NOT NULL, "
                              "password_hash TEXT NOT NULL, "
                              "salt TEXT NOT NULL, "
                              "role TEXT DEFAULT 'user', "
                              "created_time TIMESTAMP DEFAULT CURRENT_TIMESTAMP)");
    if (!success) {
        logger.log(QtCriticalMsg, QString("Ошибка создания таблицы Users: %1").arg(query.lastError().text()));
    }
}

ClientDataBase::~ClientDataBase() {
    if (db.isOpen()) {
        db.close();
    }
}

/**
 * @brief Создает нового пользователя в базе данных.
 * Добавляет запись о пользователе в таблицу Users.
 * @param firstName Имя пользователя.
 * @param lastName Фамилия пользователя.
 * @param username Уникальное имя пользователя.
 * @param password_hash Хеш пароля.
 * @param salt Соль для хеширования.
 * @return true, если пользователь успешно создан, иначе false.
 */
bool ClientDataBase::createUser(const QString& firstName, const QString& lastName, const QString& username,const QString& password_hash ,const QString& salt ) {
    QSqlQuery query(db);
    Logger& logger = Logger::getInstance();
    query.prepare("INSERT INTO Users (first_name, last_name, username,password_hash,  salt) "
                  "VALUES (:first_name, :last_name, :username, :password_hash, :salt)");
    query.bindValue(":first_name", firstName);
    query.bindValue(":last_name", lastName);
    query.bindValue(":username", username);
    query.bindValue(":password_hash", password_hash);
    query.bindValue(":salt", salt);

    if (!query.exec()) {
        logger.log(QtWarningMsg, QString("Ошибка создания пользователя '%1': %2")
                                     .arg(username).arg(query.lastError().text()));
        return false;
    }

    return true;
}

/**
 * @brief Удаляет пользователя из базы данных.
 * Удаляет запись о пользователе из таблицы Users по логину.
 * @param username логин пользователя.
 * @return true, если пользователь успешно удален, иначе false.
 */
bool ClientDataBase::deleteUser(const QString& username){
    Logger& logger = Logger::getInstance();
    QSqlQuery query(db);
    query.prepare("DELETE FROM Users WHERE username = :username");
    query.bindValue(":username", username);
    if(!query.exec()){
        logger.log(QtWarningMsg, QString("Ошибка удаления пользователя '%1': %2")
                                     .arg(username).arg(query.lastError().text()));
        return false;
    } else {
        logger.log(QtInfoMsg, QString("Пользователь '%1' успешно удален.").arg(username));
        return true;
    }
}

/**
 * @brief Проверяет существование пользователя в базе данных.
 * Выполняет запрос к таблице Users для проверки наличия пользователя.
 * @param username логин пользователя.
 * @return true, если пользователь существует, иначе false.
 */
bool ClientDataBase::existsUser(const QString& username){
    Logger& logger = Logger::getInstance();
    QSqlQuery query(db);
    query.prepare("SELECT COUNT(*) FROM Users WHERE username = :username");
    query.bindValue(":username", username);

    if (!query.exec()) {
        logger.log(QtWarningMsg,
                   QString("Ошибка выполнения запроса "
                           "для проверки существования пользователя '%1': %2")
                                     .arg(username).arg(query.lastError().text()));
        return false;
    }

    if (query.next()) {
        int count = query.value(0).toInt();
        return (count > 0);
    }

    return false;

}

/**
 * @brief Получает данные пользователя из базы данных.
 * Возвращает данные пользователя в виде QMap, где ключи соответствуют полям таблицы Users.
 * @param username логин пользователя.
 * @return QMa.p
 */
QMap<QString, QString> ClientDataBase::getUserData(const QString& username) const {
    QMap<QString, QString> userData;
    Logger& logger = Logger::getInstance();
    QSqlQuery query(db);
    query.prepare("SELECT id, first_name, last_name, username, salt, password_hash, role, created_time FROM Users WHERE username = :username");
    query.bindValue(":username", username);

    if (!query.exec()) {
        logger.log(QtWarningMsg, QString("Ошибка получения данных пользователя '%1': %2")
                                     .arg(username).arg(query.lastError().text()));
        return userData;
    }

    if (query.next()) {
        userData["id"] = query.value("id").toString();
        userData["firstName"] = query.value("first_name").toString();
        userData["lastName"] = query.value("last_name").toString();
        userData["username"] = query.value("username").toString();
        userData["password_hash"] = query.value("password_hash").toString();
        userData["salt"] = query.value("salt").toString();
        userData["role"] = query.value("role").toString();
        userData["created_time"] = query.value("created_time").toString();
    }

    return userData;
}


bool ClientDataBase::isOpen() const {
    return db.isOpen();
}


