#include "newuser.h"
#include "ui_newuser.h"

newUser::newUser(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::newUser)
{
    ui->setupUi(this);
    ui->server_port->setPlaceholderText("server:port");
    ui->username->setPlaceholderText("username");
    ui->passwd->setPlaceholderText("password");
    ui->repasswd ->setPlaceholderText("rewrite password");
    ui->server_port->setFocus();
}

newUser::~newUser()
{
    delete ui;
}

void newUser::on_ok_clicked()
{
    QString QS_server_port = ui->server_port->text().trimmed();
    QString QS_username = ui->username->text().trimmed();
    QString QS_passwd = ui->passwd->text();
    QString QS_repasswd = ui->repasswd->text();

    if(nullptr == QS_server_port){
        ui->server_port->setFocus();
        QMessageBox::warning(this, tr("Warning"),
                             tr("server & port is null!"),
                             QMessageBox::Yes);
    } else if (nullptr == QS_username) {
        ui->username->setFocus();
        QMessageBox::warning(this, tr("Warning"),
                             tr("username is null!"),
                             QMessageBox::Yes);
    } else if (nullptr == QS_passwd) {
        ui->passwd->setFocus();
        QMessageBox::warning(this, tr("Warning"),
                             tr("password is null!"),
                             QMessageBox::Yes);
    } else if (nullptr == QS_repasswd) {
        ui->repasswd->setFocus();
        QMessageBox::warning(this, tr("Warning"),
                             tr("password is null!"),
                             QMessageBox::Yes);
    } else if (0 != QS_passwd.compare(QS_repasswd)) {
        ui->passwd->clear();
        ui->repasswd->clear();
        ui->passwd->setFocus();
        QMessageBox::warning(this, tr("Warning"),
                             tr("password is inconpable!"),
                             QMessageBox::Yes);
    } else {
        // read data and create user
        char *server_port = QS_server_port.toLatin1().data();
        parserServerPort(server_port);

        // connect server
        SocketClient socketClient = SocketClient(g_server, g_port);

        // get username and passwd
        MSG send{};
        send.msgOp = CreateUser;
        strcpy(send.userName, QS_username.toLatin1().data());
        strcpy(send.passwd, QS_passwd.toLatin1().data());

        ret = socketClient.sendMsg(&send, sizeof(send));

        MSG recv{};
        ret = socketClient.recvMsg(&recv, sizeof(recv));

        if (recv.msgState == CreateUserSuccess) {
            isLogin = true;
            strcpy(g_username, send.userName);
            strcpy(g_passdw, send.passwd);
            accept();
        } else if (recv.msgState == CreateUserUserExists) {
            ui->username->clear();
            ui->username->setFocus();
            QMessageBox::warning(this, tr("Warning"),
                                 tr("username is exists!"),
                                 QMessageBox::Yes);
        } else if (recv.msgState == CreateUserUndefinedError) {
            QMessageBox::warning(this, tr("Warning"),
                                 tr("server undefined error!"),
                                 QMessageBox::Yes);
        } else {
            QMessageBox::warning(this, tr("Warning"),
                                 tr("client undefined error!"),
                                 QMessageBox::Yes);
        }
    }
}

void newUser::on_cancel_clicked()
{
    reject();
}