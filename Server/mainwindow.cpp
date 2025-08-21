#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QDebug>
#include "ChatManager.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include "logger.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    loadSettings();
    managerNetwork = new ManagerNetwork(this);

    connect(ui->user_bd_2, &QPushButton::clicked, this, &MainWindow::onUserDbButtonClicked);
    connect(ui->Chat_bd2, &QPushButton::clicked, this, &MainWindow::onChatDbButtonClicked);
    connect(ui->Log_bd2, &QPushButton::clicked, this, &MainWindow::onLogButtonClicked);
    connect(ui->CreateChatpushButton, &QPushButton::clicked, this, &MainWindow::onCreateChatButtonClicked);

    connect(ui->DeleteChatpushButton, &QPushButton::clicked, this, &MainWindow::onDeleteChatButtonClicked);
}

MainWindow::~MainWindow()
{
    delete ui;
    Logger::getInstance().close();

}

void MainWindow::on_StartServer_clicked()
{
    Logger& logger = Logger::getInstance();
    connect(&Logger::getInstance(), &Logger::logReceived, this, &MainWindow::appendLogToInterface);
    QString logFilePath = ui->Log_bd->text().trimmed();

    qDebug() <<logFilePath;

    if (logFilePath.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Укажите путь к файлу журнала событий");
        return;
    }

    logger.setLogFile(logFilePath);
    logger.open();
    if (!logger.isOpen()) {
        logger.createLogFile();
    }
    logger.log(QtInfoMsg, "Запуск сервера");



    quint16 port = ui->Port->value();
    QString ipStr = ui->ip_adres_lissen->text().trimmed();
    QHostAddress address;
    if (ipStr.isEmpty() || ipStr == "0.0.0.0") {
        address = QHostAddress::Any;
    } else {
        address = QHostAddress(ipStr);
        if (address.isNull()) {
            QMessageBox::critical(this, "Ошибка", "Неверный IP-адрес");
            return;
        }
    }



    QString userDbPath = ui->user_bd->text().trimmed();
    QString chatDbPath = ui->Chat_bd->text().trimmed();

    if (userDbPath.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Укажите путь к базе данных пользователей");
        return;
    }
    if (chatDbPath.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Укажите путь к базе данных чатов");
        return;
    }

    clientDataBase = new ClientDataBase(userDbPath, this);
    if (!clientDataBase->isOpen()) {
        QMessageBox::critical(this, "Ошибка", "Не удалось открыть базу данных пользователей");
        delete clientDataBase;
        clientDataBase = nullptr;
        return;
    }

    chatManager = new ChatManager(chatDbPath, this);  // предполагается, что конструктор принимает путь
    if (!chatManager->isOpen()) {
        QMessageBox::critical(this, "Ошибка", "Не удалось открыть базу данных чатов");
        delete chatManager;
        chatManager = nullptr;
        delete clientDataBase;
        clientDataBase = nullptr;
        return;
    }

    QStringList chatNames = chatManager->getAllChatNames();
    for (const QString& name : chatNames) {
        ui->LisyOfChatListWidget->addItem(name);
        ui->ChatSelectionComboBox->addItem(name);
    }

    packetRouter = new PacketRouter(this);

    PacketRegisterHandler* packetRegisterHandler = new PacketRegisterHandler(clientDataBase, managerNetwork, this);
    PacketAuthHandler* packetAuthHandler = new PacketAuthHandler(clientDataBase, managerNetwork, this);
    PacketMessageHandler* packetMessageHandler = new PacketMessageHandler(clientDataBase, managerNetwork, chatManager,  this);
    PacketChatListHandler* chatListHandler = new PacketChatListHandler(chatManager, managerNetwork, this);
    packetRouter->registerHandler(packetRegisterHandler);
    packetRouter->registerHandler(packetAuthHandler);
    packetRouter->registerHandler(packetMessageHandler);
    packetRouter->registerHandler(chatListHandler);
    connect(managerNetwork, &ManagerNetwork::dataReceived, this, &MainWindow::onDataReceived);
    connect(managerNetwork, &ManagerNetwork::errorOccurred, this, &MainWindow::handleServerError);
    connect(managerNetwork, &ManagerNetwork::newConnection, this, &MainWindow::handleNewConnection);
    connect(managerNetwork, &ManagerNetwork::clientDisconnected, this, &MainWindow::handleClientDisconnected);
    connect(chatManager, &ChatManager::chatDeleted, this, &MainWindow::sendUpdatedChatList);
    connect(chatManager, &ChatManager::chatAdded, this, &MainWindow::sendUpdatedChatList);
    saveSettings();
    managerNetwork->startServer(port, address);
    logger.log(QtInfoMsg, QString("Сервер успешно запустился на порту %1 и слушает %2").arg(port).arg(address.toString()));

}

void MainWindow::handleServerError(const QString& errorMessage) {
}

void MainWindow::handleNewConnection(QTcpSocket* socket) {
}



void MainWindow::handleClientDisconnected(QTcpSocket* socket) {
    qDebug() << "Клиент отключился:" << socket->peerAddress().toString();
}

void MainWindow::onDataReceived(QTcpSocket* socket, const QByteArray& data) {
    packetRouter->routePacket(socket, data);
}

void MainWindow::on_Port_valueChanged(int arg1)
{

}


void MainWindow::on_ip_adres_lissen_textChanged(const QString &arg1)
{

}


void MainWindow::on_user_bd_textEdited(const QString &arg1)
{

}


void MainWindow::on_Chat_bd_textEdited(const QString &arg1)
{

}


void MainWindow::on_Log_bd_textEdited(const QString &arg1)
{

}
/*
void MainWindow::on_pushButton_3_clicked()
{

}
*/

void MainWindow::loadSettings()
{
    QSettings settings("Grachev", "ChatServer");
    ui->Port->setValue(settings.value("port", 3333).toInt());
    ui->ip_adres_lissen->setText(settings.value("ip", "0.0.0.0").toString());
    ui->user_bd->setText(settings.value("user_db_path", "").toString());
    ui->Chat_bd->setText(settings.value("chat_db_path", "").toString());
    ui->Log_bd->setText(settings.value("log_file_path", "").toString());
}
void MainWindow::saveSettings()
{
    QSettings settings("Grachev", "ChatServer");
    settings.setValue("port", ui->Port->value());
    settings.setValue("ip", ui->ip_adres_lissen->text());
    settings.setValue("user_db_path", ui->user_bd->text());
    settings.setValue("chat_db_path", ui->Chat_bd->text());
    settings.setValue("log_file_path", ui->Log_bd->text());
}

void MainWindow::on_ChatSelectionComboBox_currentTextChanged(const QString &chatName)
{
    if (chatName.isEmpty() || !chatManager) {
        ui->chatHistory->clear();
        return;
    }

    Chat* chat = chatManager->getChat(chatName);
    if (chat->getName() == nullptr) {
        ui->chatHistory->setText("Чат не найден.");
        return;
    }

    ui->chatHistory->clear();
    const auto& messages = chat->getMessages();
    for (const auto& msg : messages) {
        QString messageText = QString("<b>[%1]</b> <i>%2:</i> %3")
        .arg(msg.getTimestamp().toString("dd-hh:mm"), msg.getSender(), msg.getText());
        ui->chatHistory->append(messageText);
    }
}

void MainWindow::sendUpdatedChatList() {
    Logger& logger = Logger::getInstance();
    PacketChatList packet;
    QStringList chatNames = chatManager->getAllChatNames();
    packet.setChatNames(chatNames);

    QByteArray serializedData = packet.serialize();
    managerNetwork->broadcastMessage(serializedData);
    logger.log(QtInfoMsg, "Обновленный список чатов отправлен.");
}

void MainWindow::updateChatListUI() {
    ui->LisyOfChatListWidget->clear();
    ui->ChatSelectionComboBox->clear();

    QStringList chatNames = chatManager->getAllChatNames();

    for (const QString& name : chatNames) {
        ui->LisyOfChatListWidget->addItem(name);
        ui->ChatSelectionComboBox->addItem(name);
    }
}

void MainWindow::onDeleteChatButtonClicked() {
    QString name = ui->DeleteChatlineEdit->text().trimmed();
    if (name.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите название чата для удаления");
        return;
    }
    if (!chatManager->getAllChatNames().contains(name)) {
        QMessageBox::warning(this, "Ошибка", QString("Чат '%1' не найден.").arg(name));
        return;
    }

    if (chatManager->deleteChat(name)) {
        updateChatListUI();
        ui->DeleteChatlineEdit->clear();
        Logger::getInstance().log(QtInfoMsg, QString("Чат '%1' успешно удален через интерфейс.").arg(name));
    } else {
        Logger::getInstance().log(QtWarningMsg, QString("Не удалось удалить чат '%1'.").arg(name));
    }
}

void MainWindow::onUserDbButtonClicked() {
    QString path = QFileDialog::getOpenFileName(
        this,
        "Выберите базу данных пользователей",
        "",
        "SQLite (*.sqlite *.db);;All Files (*.*)"
        );
    if (!path.isEmpty()) {
        ui->user_bd->setText(path);
    }
}

void MainWindow::onChatDbButtonClicked() {
    QString path = QFileDialog::getOpenFileName(
        this,
        "Выберите базу данных чатов",
        "",
        "SQLite (*.sqlite *.db);;All Files (*.*)"
        );
    if (!path.isEmpty()) {
        ui->Chat_bd->setText(path);
    }
}

void MainWindow::onLogButtonClicked() {
    QString path = QFileDialog::getOpenFileName(
        this,
        "Выберите файл журнала событий",
        "",
        "Log Files (*.log);;Text Files (*.txt);;All Files (*.*)"
        );
    if (!path.isEmpty()) {
        ui->Log_bd->setText(path);
    }
}

void MainWindow::onCreateChatButtonClicked() {
    QString name = ui->CreateChatlineEdit->text().trimmed();
    if (name.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите название чата");
        return;
    }

    chatManager->createChat(name);
    ui->LisyOfChatListWidget->addItem(name);
    ui->ChatSelectionComboBox->addItem(name);
    ui->CreateChatlineEdit->clear();
}
void MainWindow::appendLogToInterface(const QString& logMessage) {
    ui->LogsTextEdit->append(logMessage);
}
