#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "AuthManager.h"
#include "managernetwork.h"
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_AuthPahe1PushButton_clicked();

    void on_Go0Page_clicked();

    void on_RegisterPage1PushButton_2_clicked();

    void on_RegisterButton_clicked();

    void on_Text_Name_RegisterPage_textChanged(const QString &arg1);

    void on_Text_Login_RegisterPage_textChanged(const QString &arg1);

    void on_Text_Password1_RegisterPage_textChanged(const QString &arg1);

    void on_Text_Password2_RegisterPage_textChanged(const QString &arg1);

    void handleSuccessfulAuth();
    void handleAuthFailure(const QString &message);
    void handleSuccessfulRegistration();

private:
    Ui::MainWindow *ui;
    ManagerNetwork *managerNetwork;
    AuthManager *authManager;

};
#endif // MAINWINDOW_H
