#pragma once

#include <QMap>
#include <QStandardItemModel>
#include "Chat.h"
#include "ChatDatabase.h"


class ChatManager : public QObject {
    Q_OBJECT

private:
    QMap<QString, Chat> chats;
    QStandardItemModel model;
    ChatDatabase database;

public:
    ChatManager(const QString& dbPath, QObject* parent = nullptr);

    void createChat(const QString& name);
    Chat* getChat(const QString& name);
    void addMessageToChat(const QString& chatName, const QString& sender, const QString& text,
                     const QDateTime& timestamp, const QString& firstName, const QString& lastName);
    bool isOpen() const { return database.isOpen(); }
    void loadChat(const QString& name);

    QStringList getAllChatNames() const;
    QStandardItemModel* getModel() { return &model; }

signals:
    void chatUpdated(const QString& chatName);
    void chatDeleted();
};
