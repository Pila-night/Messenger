#include "mainwindow.h"

#include <QApplication>
#include "protocol.h"
int main(int argc, char *argv[])
{

    Register a;
    a.setUsername("Viktor");
    a.setPassword("Пароль");
    QByteArray ser = a.serialize();
    qDebug() << "Сериализованный пакет: " <<ser;

    Message m;
    m.setFrom("ЧАТ 1");
    m.setText("Го в валик сегодня в 19:00 поиграем?");
    QByteArray mes = m.serialize();

    std::unique_ptr<Packet> deserialized = Packet::deserialize(ser);
    switch (deserialized -> getType()){
    case PacketType::Register : {
        Register* registerPacket = static_cast<Register*>(deserialized.get());
        QString username = registerPacket->getUsername();
        QString password = registerPacket->getPassword();
        qDebug() << "От клиента пришли следующие данные: " << username << password;
        break;
    }
    default :
    {
        qDebug() << "Клиент подсунул не тот тип!";
    }
    }

    deserialized = Packet::deserialize(mes);
    switch (deserialized -> getType()){
    case PacketType::Register : {
        Register* registerPacket = static_cast<Register*>(deserialized.get());
        QString username = registerPacket->getUsername();
        QString password = registerPacket->getPassword();
        qDebug() << "От клиента пришли следующие данные: " << username << password;
        break;
    }
    case PacketType::Message:{
        Message* Message1 = static_cast<Message*>(deserialized.get());
        QString from = Message1->getFrom();
        QString text = Message1->getText();
        qDebug() << "От клиента пришли следующие данные: " << from << text;
        break;
    }
    default :
    {
        qDebug() << "Клиент подсунул не тот тип!";
    }
    }


   /* QApplication a(argc, argv);
    MainWindow w;
    w.show();*/
    return 0 /*a.exec()*/;
}
