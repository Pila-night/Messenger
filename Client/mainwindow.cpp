#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Создаю менеджер сети
    managerNetwork = new ManagerNetwork(this);

    // Подключаемся к серверу
    managerNetwork->connectToServer("127.0.0.1", 3333);

    // Создаем AuthManager
    authManager = new AuthManager(managerNetwork, this);

    //  Подключаем сигналы
    connect(authManager, &AuthManager::okAuthUser, this, &MainWindow::handleSuccessfulAuth);
    connect(authManager, &AuthManager::authFail, this, &MainWindow::handleAuthFailure);
    connect(authManager, &AuthManager::okRegisterUser, this, &MainWindow::handleSuccessfulRegistration);

    connect(ui->stackedWidget, &QStackedWidget::currentChanged, this, [this](int index) {
        Q_UNUSED(index);
        ui->Error_Label_Register_Page->clear();
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_AuthPahe1PushButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
    this->adjustSize();
}


void MainWindow::on_Go0Page_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
    this->adjustSize();
}


void MainWindow::on_RegisterPage1PushButton_2_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
    this->adjustSize();
}


void MainWindow::on_RegisterButton_clicked()
{
    QString username = ui->Text_Login_RegisterPage->text() ;
    QString password1 = ui->Text_Password1_RegisterPage->text() ;
    QString password2 = ui->Text_Password2_RegisterPage->text() ;

    qDebug() << "Получены данные с полей";

    if(username.isEmpty() || password1.isEmpty() || password2.isEmpty()){
        ui->Error_Label_Register_Page->setText("Заполните все поля");
        return;
    }
    qDebug() << "Все поля заполнены";

    if (password1 != password2){
        ui->Error_Label_Register_Page->setText("Пароли не совпали");
        return;
    }

    qDebug() << "Пароли совпали";
    qDebug() << "Начинаю авторизацию";

    authManager->registerUser(username, password1);

}





void MainWindow::handleSuccessfulAuth()
{

    ui->stackedWidget->setCurrentIndex(3);
}

void MainWindow::handleAuthFailure(const QString &message)
{
    ui->Error_Label_Register_Page->setText(message);
}

void MainWindow::handleSuccessfulRegistration()
{
    ui->stackedWidget->setCurrentIndex(0);
    ui->Error_Label_Register_Page->setText("Регистрация успешна. Авторизуйтесь");
}



/*Для страницы регистрации, чтобы ошибка исчезала*/

void MainWindow::on_Text_Name_RegisterPage_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    ui->Error_Label_Register_Page->clear();
}


void MainWindow::on_Text_Login_RegisterPage_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    ui->Error_Label_Register_Page->clear();
}


void MainWindow::on_Text_Password1_RegisterPage_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    ui->Error_Label_Register_Page->clear();
}


void MainWindow::on_Text_Password2_RegisterPage_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    ui->Error_Label_Register_Page->clear();
}

