#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "protocol.h"
#include "Packetrouter.h"
#include "packethandler.h"
#include "Packetrouter.h"
#include "logger.h"
#include "SecurityUtils.h"
#include <QStandardPaths>
#include <QCloseEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);


    chatListModel = new QStandardItemModel(this);

    QString dbPath = QCoreApplication::applicationDirPath() + "/Messenger.db";
    chatManager = new ChatManager(dbPath, this);

    ui->ChatList->setModel(chatListModel);


    //сигналы для списка чатов*/
    connect(ui->ChatList, &QListView::clicked, this, &MainWindow::on_ChatList_clicked);


    // =====================
    /*Создание обработчиков пакетов*/
    PacketServerResponseHandler* serverResponseHandler = new PacketServerResponseHandler(this);
    PacketMessageHandler* messageHandler = new PacketMessageHandler(this);
    PacketChatListHandler* chatListHandler = new PacketChatListHandler(this);

    /*Создание маршрутизатора пакетов*/
    packetRouter = new PacketRouter(this);

    /*Регистрация обработчиков в роутере*/
    packetRouter->registerHandler(serverResponseHandler);
    packetRouter->registerHandler(messageHandler);
    packetRouter->registerHandler(chatListHandler);

    /* Создание менеджера сети*/
    managerNetwork = new ManagerNetwork(this);
    //managerNetwork->connectToServer("127.0.0.1", 3333);

    /*сигнал для получения данных из сети*/
    connect(managerNetwork, &ManagerNetwork::dataReceived, this, &MainWindow::onDataReceived);

    /*сигналы для обработчика ответов сервера*/
    connect(serverResponseHandler, &PacketServerResponseHandler::authSaltReceived,
            this, &MainWindow::on_SaltReceived);

    connect(serverResponseHandler, &PacketServerResponseHandler::authSuccess,
            this, &MainWindow::handleSuccessfulAuth);

    connect(serverResponseHandler, &PacketServerResponseHandler::authFailed,
            this, &MainWindow::handleAuthFailure);

    connect(serverResponseHandler, &PacketServerResponseHandler::RegisterFailed,
            this, &MainWindow::handleRegisterFailed);

    /*сигнал для получения сообщений*/
    connect(messageHandler, &PacketMessageHandler::messageReceived,
            this, &MainWindow::onMessageReceived);

    /*сигнал для получения списка чатов*/
    connect(chatListHandler, &PacketChatListHandler::chatListReceived,
            this, &MainWindow::onChatListReceived);

    /* === Очистка ошибок при вводе === */
    connect(ui->login, &QLineEdit::textChanged, this, [this](const QString&) {
        ui->ErrorLabel->clear();
    });

    connect(ui->password, &QLineEdit::textChanged, this, [this](const QString&) {
        ui->ErrorLabel->clear();
    });

    connect(ui->stackedWidget, &QStackedWidget::currentChanged, this, [this](int index) {
        Q_UNUSED(index);
        ui->Error_Label_Register_Page->clear();
    });

    connect(chatManager, &ChatManager::chatUpdated, this, [this](const QString& chatName) {
        if (currentChatName == chatName) {
            loadChatHistory(chatName);
        }
    });
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::on_AuthPahe1PushButton_clicked() {
    Logger& logger = Logger::getInstance();
    username = ui->login->text();
    QString password = ui->password->text();

    if (username.isEmpty() || password.isEmpty()) {
        logger.log(QtWarningMsg, "Попытка авторизации без заполнения логина или пароля.");
        ui->ErrorLabel->setText("Введите логин и пароль");
        return;
    }

    PacketAuth packet;
    packet.setUsername(username);
    packet.setPassword(password);
    logger.log(QtInfoMsg, "Попытка подключения к серверу для авторизации");
    managerNetwork->connectToServer("127.0.0.1", 3333);
    managerNetwork->sendPacket(packet.serialize());
}

void MainWindow::on_SaltReceived(const QString &salt) {
    this->salt = salt;

    QString username = ui->login->text();
    QString password = ui->password->text();
    QString hash = SecurityUtils::hashPassword(password, salt);

    PacketAuth packet;
    packet.setUsername(username);
    packet.setPassword(hash);

    managerNetwork->sendPacket(packet.serialize());
}

void MainWindow::handleSuccessfulAuth() {
    ui->stackedWidget->setCurrentIndex(1);
    ui->ErrorLabel->clear();

    PacketChatList packet;
    managerNetwork->sendPacket(packet.serialize());
}

void MainWindow::handleAuthFailure(const QString &message) {
    ui->ErrorLabel->setText(message);
}


void MainWindow::handleRegisterFailed(const QString &message) {
    ui->Error_Label_Register_Page->setText(message);
}

void MainWindow::on_Go0Page_clicked() {
    ui->stackedWidget->setCurrentIndex(0);
    this->adjustSize();
}

void MainWindow::on_RegisterPage1PushButton_2_clicked() {
    ui->stackedWidget->setCurrentIndex(2);
    this->adjustSize();
}

void MainWindow::on_RegisterButton_clicked() {
    QString username = ui->Text_Login_RegisterPage->text();
    QString firstName = ui->firstName->text();
    QString lastName = ui->lastName->text();
    QString password1 = ui->Text_Password1_RegisterPage->text();
    QString password2 = ui->Text_Password2_RegisterPage->text();


    if (username.isEmpty() || firstName.isEmpty() || lastName.isEmpty() || password1.isEmpty() || password2.isEmpty()) {
        ui->Error_Label_Register_Page->setText("Заполните все поля");
        return;
    }

    if (password1 != password2) {
        ui->Error_Label_Register_Page->setText("Пароли не совпали");
        return;
    }

    PacketRegister registerPacket;
    registerPacket.setUsername(username);
    registerPacket.setPassword(password1);
    registerPacket.setFirst_name(firstName);
    registerPacket.setLast_name(lastName);

    managerNetwork->connectToServer("127.0.0.1", 3333);
    QByteArray serializedRegister = registerPacket.serialize();
    managerNetwork->sendPacket(serializedRegister);

    ui->stackedWidget->setCurrentIndex(0);
    this->adjustSize();

}

void MainWindow::handleSuccessfulRegistration() {
    ui->stackedWidget->setCurrentIndex(0);
    ui->Error_Label_Register_Page->setText("Регистрация успешна. Авторизуйтесь");
}

void MainWindow::handleRegistrationFailure() {
    ui->stackedWidget->setCurrentIndex(0);
    ui->Error_Label_Register_Page->setText("Регистрация не удалась. Попробуйте снова.");
}

void MainWindow::on_ChatList_clicked(const QModelIndex& index) {
    QString chatName = index.data().toString();
    currentChatName = chatName;
    loadChatHistory(chatName);
}

void MainWindow::loadChatHistory(const QString& chatName) {
    Chat* chat = chatManager->getChat(chatName);
    if (chat->getName() == nullptr) {
        return;
    }

    ui->textBrowser->clear();



    const auto& messages = chat->getMessages();
    for (const auto& msg : messages) {
        QString messageText = QString("<b>[%1]</b> <i>%2 %3(%4):</i> %5")
        .arg(msg.getTimestamp().toString("dd-hh:mm"),
             msg.getFirstName(),
             msg.getLastName(),
             msg.getSender(),
             msg.getText());

        ui->textBrowser->append(messageText);
    }
}

void MainWindow::on_SendMessageButton_clicked() {
    QString text = ui->MessageInput->text().trimmed();
    if (text.isEmpty() || currentChatName.isEmpty()) {
        return;
    }
    PacketMessage mes;
    mes.setChatName(currentChatName);
    mes.setFrom(username);
    mes.setText(text);

    managerNetwork->sendPacket(mes.serialize());
    ui->MessageInput->clear();
}

void MainWindow::onMessageReceived(const QString& firstName,const QString& lastName, const QString& chatName, const QString& sender, const QString& text, const QDateTime& timestamp) {
    chatManager->addMessageToChat(chatName, sender, text, timestamp, firstName, lastName);
        if (currentChatName == chatName) {
        loadChatHistory(chatName);
    }
}

void MainWindow::onChatListReceived(const QStringList& chatList) {
    chatListModel->clear();
    for (const QString& chatName : chatList) {
        chatManager->createChat(chatName);
        QStandardItem* item = new QStandardItem(chatName);
        chatListModel->appendRow(item);
    }
}

void MainWindow::onDataReceived(const QByteArray& data) {
    packetRouter->routePacket(data);
}

void MainWindow::on_Text_Name_RegisterPage_textChanged(const QString &arg1) {
    Q_UNUSED(arg1);
    ui->Error_Label_Register_Page->clear();
}

void MainWindow::on_Text_Login_RegisterPage_textChanged(const QString &arg1) {
    Q_UNUSED(arg1);
    ui->Error_Label_Register_Page->clear();
}

void MainWindow::on_Text_Password1_RegisterPage_textChanged(const QString &arg1) {
    Q_UNUSED(arg1);
    ui->Error_Label_Register_Page->clear();
}

void MainWindow::on_Text_Password2_RegisterPage_textChanged(const QString &arg1) {
    Q_UNUSED(arg1);
    ui->Error_Label_Register_Page->clear();
}

void MainWindow::on_ChatList_indexesMoved(const QModelIndexList &indexes) {
}

void MainWindow::closeEvent(QCloseEvent *event) {
    Logger& logger = Logger::getInstance();

    logger.log(QtInfoMsg, "Пользователь инициировал закрытие приложения.");


    if (managerNetwork && managerNetwork->isConnected()) {
        managerNetwork->disconnectFromServer();
    }

    event->accept();
}
