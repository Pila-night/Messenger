#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include "ManagerNetwork.h"
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
    void on_StartServer_clicked();
    void handleServerError(const QString& errorMessage); // Обработка ошибок сервера
    void handleNewConnection(QTcpSocket* socket);        // Обработка новых подключений
    void onDataReceived(QTcpSocket* socket, const QByteArray& data); // Обработка данных
    void handleClientDisconnected(QTcpSocket* socket);
    void on_Port_valueChanged(int arg1);

    void on_ip_adres_lissen_textChanged(const QString &arg1);

    void on_user_bd_textEdited(const QString &arg1);

    void on_Chat_bd_textEdited(const QString &arg1);


    void on_ChatSelectionComboBox_currentTextChanged(const QString &chatName);

    void on_Log_bd_textEdited(const QString &arg1);

    void sendUpdatedChatList();
    void appendLogToInterface(const QString& logMessage);


    //void on_pushButton_3_clicked();

private:
    Ui::MainWindow *ui;
    ManagerNetwork* managerNetwork;
    PacketRouter* packetRouter;
    ClientDataBase *clientDataBase;
    ChatManager *chatManager;

    void loadSettings();
    void saveSettings();
    void onDeleteChatButtonClicked();
    void updateChatListUI();
    void onUserDbButtonClicked();
    void onChatDbButtonClicked();
    void onLogButtonClicked();
    void onCreateChatButtonClicked();


    void setupFileDialogConnections();
    void setupLogOutput();
};
#endif // MAINWINDOW_H
