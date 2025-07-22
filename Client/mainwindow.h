#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "AuthManager.h"
#include "managernetwork.h"
#include "ChatManager.h"
#include <QItemSelectionModel>
#include <QStandardItemModel>
#include "Packetrouter.h"

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

    void handleRegistrationFailure();

    void on_SaltReceived(const QString &salt);

    void on_ChatList_indexesMoved(const QModelIndexList &indexes);

    void loadChatHistory(const QString& chatName);
    void on_ChatList_clicked(const QModelIndex& index);
    void on_SendMessageButton_clicked();
    void onMessageReceived(const QString& chatName, const QString& sender, const QString& text, const QDateTime& timestamp);
    void onChatListReceived(const QStringList& chatList);

    void onDataReceived(const QByteArray& data);
private:
    QString username;
    QString currentChatName;
    QString salt;
    QStandardItemModel* chatListModel;
    Ui::MainWindow *ui;
    ManagerNetwork *managerNetwork;
    AuthManager *authManager;
    ChatManager* chatManager;
    PacketRouter* packetRouter;
  // messageHandler;*/

};
#endif // MAINWINDOW_H
