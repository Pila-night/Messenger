#include "mainwindow.h"
#include <QApplication>
#include "protocol.h"
#include <QDebug>

int main(int argc, char *argv[]) {


    PacketChatList ChatList;

    QStringList a = {"макасие",":"};
    QStringList b = {":","надшка"};
    ChatList.setChatNames(a);
    ChatList.setChatNames(b);

    QStringList result = ChatList.getChatNames();

    qDebug() << result;

    QByteArray cl = ChatList.serialize();
    qDebug() << "Сериализованный Чат листа:" << cl ;

    std::shared_ptr<Packet> deserialized2 = Packet::deserialize(cl);

    qDebug() << "Пережил десериализацию:" << cl;

    auto resp = std::dynamic_pointer_cast<PacketChatList>(deserialized2);

    qDebug() << "Начинаю получать имя:";
    QStringList res = resp->getChatNames();

    qDebug() << "Результат десериализации пакета с листом чатов" <<res ;

    PacketMessage mes;
    mes.setFrom("Аьамэ");
    mes.setText("F,fv' kj[");
    mes.setChatName("Арбузеры");
    mes.setTimestamp(QDateTime::currentDateTime());

    QByteArray smes = mes.serialize();
    std::shared_ptr<Packet> deserialized3 = Packet::deserialize(smes);
    auto resp2 = std::dynamic_pointer_cast<PacketMessage>(deserialized3);

    qDebug() <<resp2->getFrom();
    qDebug() <<resp2->getChatName();
    qDebug() <<resp2->getText();
    qDebug() <<resp2->getTimestamp();



    return 0;/*
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    a.exec();
    return 0;*/
}
