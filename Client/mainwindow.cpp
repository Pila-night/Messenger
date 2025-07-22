#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "protocol.h"
#include "Packetrouter.h"
#include "packethandler.h"
#include "AuthManager.h"
#include "Packetrouter.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);


    chatListModel = new QStandardItemModel(this);
    chatManager = new ChatManager(chatListModel, this);

    //для проверки функционирования чатов
    chatManager->createChat("Чат 1");
    chatManager->createChat("Чат 2");
    chatManager->createChat("Работа");
    chatManager->createChat("Друзья");

    ui->ChatList->setModel(chatListModel);

    //сигналы для списка чатов*/
    connect(ui->ChatList, &QListView::clicked, this, &MainWindow::on_ChatList_clicked);
    connect(chatManager, &ChatManager::chatUpdated, this, [this](const QString& chatName) {
        if (currentChatName == chatName) {
            loadChatHistory(chatName);
        }
    });

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
    managerNetwork->connectToServer("127.0.0.1", 3333);
    authManager = new AuthManager(this);

    /* сигналы для авторизации*/
    connect(authManager, &AuthManager::okAuthUser, this, &MainWindow::handleSuccessfulAuth);
    connect(authManager, &AuthManager::authFail, this, &MainWindow::handleAuthFailure);
    connect(authManager, &AuthManager::okRegisterUser, this, &MainWindow::handleSuccessfulRegistration);

    /*сигнал для получения данных из сети*/
    connect(managerNetwork, &ManagerNetwork::dataReceived, this, &MainWindow::onDataReceived);

    /*сигналы для обработчика ответов сервера*/
    connect(serverResponseHandler, &PacketServerResponseHandler::authSaltReceived,
            this, &MainWindow::on_SaltReceived);

    connect(serverResponseHandler, &PacketServerResponseHandler::authSuccess,
            this, &MainWindow::handleSuccessfulAuth);

    connect(serverResponseHandler, &PacketServerResponseHandler::authFailed,
            this, &MainWindow::handleAuthFailure);

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
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::on_AuthPahe1PushButton_clicked() {
    username = ui->login->text();
    QString password = ui->password->text();

    if (username.isEmpty() || password.isEmpty()) {
        ui->ErrorLabel->setText("Введите логин и пароль");
        return;
    }

    PacketAuth packet;
    packet.setUsername(username);
    packet.setPassword("");
    managerNetwork->sendPacket(packet.serialize());
}

void MainWindow::on_SaltReceived(const QString &salt) {
    this->salt = salt;

    QString username = ui->login->text();
    QString password = ui->password->text();

    QString hash = authManager->getHashPassword(password, salt);

    PacketAuth packet;
    packet.setUsername(username);
    packet.setPassword(hash);

    managerNetwork->sendPacket(packet.serialize());
}

void MainWindow::handleSuccessfulAuth() {
    qDebug() << "Авторизация успешна";
    ui->stackedWidget->setCurrentIndex(3);
    ui->ErrorLabel->clear();
}

void MainWindow::handleAuthFailure(const QString &message) {
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
    QString password1 = ui->Text_Password1_RegisterPage->text();
    QString password2 = ui->Text_Password2_RegisterPage->text();

    qDebug() << "Получены данные с полей";

    if (username.isEmpty() || password1.isEmpty() || password2.isEmpty()) {
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

    QByteArray serializedRegister = registerPacket.serialize();
    managerNetwork->sendPacket(serializedRegister);
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
    auto chat = chatManager->getChat(chatName);
    if (!chat) {
        qWarning() << "Чат не найден:" << chatName;
        return;
    }

    ui->textBrowser->clear();

    const auto& messages = chat->getMessages();
    for (const auto& msg : messages) {
        QString messageText = QString("<b>[%1]</b> <i>%2:</i> %3")
        .arg(msg->getTimestamp().toString("dd-hh:mm"), msg->getSender(), msg->getText());
        ui->textBrowser->append(messageText);
    }
}

void MainWindow::on_SendMessageButton_clicked() {
    QString text = ui->MessageInput->text().trimmed();
    if (text.isEmpty() || currentChatName.isEmpty()) {
        return;
    }

    chatManager->addMessageToChat(currentChatName, username, text, QDateTime::currentDateTime());
    ui->MessageInput->clear();
    loadChatHistory(currentChatName);

    PacketMessage mes;
    mes.setChatName(currentChatName);
    mes.setFrom(username);
    mes.setText(text);
}

void MainWindow::onMessageReceived(const QString& chatName, const QString& sender, const QString& text, const QDateTime& timestamp) {
    chatManager->addMessageToChat(chatName, sender, text, timestamp);
        if (currentChatName == chatName) {
        loadChatHistory(chatName);
    }
}

/*Обнавление списка чатов, emit chatListReceived */
void MainWindow::onChatListReceived(const QStringList& chatList) {
    chatListModel->clear();
    for (const QString& chatName : chatList) {
        chatManager->createChat(chatName);
    }
}

/*передача данных в маршрутизатор пакетов*/
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
