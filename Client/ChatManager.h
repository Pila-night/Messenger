#pragma once

#include <QObject>
#include <QString>
#include <QStringList>
#include <QMap>
#include <memory>
#include <QStandardItemModel>
#include "Chat.h"
#include "ChatDatabase.h"

/**
 * @brief Класс ChatManager управляет чатами и сообщениями.
 * Предоставляет методы для создания чатов, добавления сообщений,
 * получения списка всех чатов и работы с базой данных для хранения сообщений.
 */

class ChatManager : public QObject {
    Q_OBJECT
private:
    QMap<QString, std::shared_ptr<Chat>> chats; /*Список чатов, где ключ - имя чата.*/
    QStandardItemModel* model; /*Модель для отображения списка чатов*/
    ChatDatabase* database; /**/

public:
     ChatManager(QStandardItemModel* listModel, QObject* parent = nullptr);

    void createChat(const QString& name);
    std::shared_ptr<Chat> getChat(const QString& name);
    void addMessageToChat(const QString& chatName, const QString& sender, const QString& text, const QDateTime& timestamp);

    QStringList getAllChatNames() const;
    QStandardItemModel* getModel() const { return model; }

signals:
    void chatUpdated(const QString& chatName);
};
