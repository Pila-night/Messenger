#ifndef CLIENTDATABASE_H
#define CLIENTDATABASE_H

#include <QObject>
#include <QSqlDatabase>
#include <QString>


class ClientDataBase : public QObject
{
    Q_OBJECT

private:

QSqlDatabase db;

public:
    ClientDataBase(const QString& path, QObject* parent = nullptr);
    ~ClientDataBase();

    bool createUser(const QString& firstName, const QString& lastName, const QString& username,const QString& password_hash ,const QString& salt );
    bool deleteUser(const QString& username);
    bool existsUser(const QString& username);
    bool isOpen() const;

    QMap<QString, QString> getUserData(const QString& username) const; /*получаем данные пользователя*/

};

#endif // CLIENTDATABASE_H
