#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
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

