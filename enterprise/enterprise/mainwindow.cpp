#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    this->secret_key_ = NULL;
    this->public_key_ = NULL;

    ui->setupUi(this);
    ui->stackedWidget->setCurrentWidget(ui->LoginPage);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_reg_registerButton_clicked()
{
    bool flag = true;

    //check if username is valid
    flag &= (ui->reg_usernameText->text() != "");

    if (!flag) {
        qDebug() << "username blank";
        return;
    }

    flag &= (ui->reg_passwordText->text() != "");

    if (!flag) {
        qDebug() << "password field is empty";
        return;
    }

    //check if two password holder is the same
    flag &= (ui->reg_passwordText->text() == ui->reg_confirmPassText->text());

    if (!flag) {
        qDebug() << "password and confirm filed differ";
        return;
    }

    //if it's ok then create key file
    std::string file_path = (QDir::currentPath() + "/keystore/" + ui->reg_usernameText->text() + ".key").toStdString();
    std::string passphrase = ui->reg_passwordText->text().toStdString();

    flag &= KeyManager::createKey(file_path, passphrase);

    if (!flag) {

        return;
    }

    ui->stackedWidget->setCurrentWidget(ui->LoginPage);
}

void MainWindow::on_reg_LoginButton_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->LoginPage);
}

void MainWindow::on_log_registerButton_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->RegisterPage);
}

void MainWindow::on_log_loginButton_clicked()
{
    bool flag = true;

    flag &= (ui->log_usernameText->text() != "");

    if (!flag) {
        qDebug() << "username text empty";
        return;
    }

    flag &= (ui->log_passwordText->text() != "");

    if (!flag) {
        qDebug() << "password text empty";
        return;
    }

    std::string file_path = (QDir::currentPath() + "/keystore/" + ui->log_usernameText->text() + ".key").toStdString();
    std::string passphrase = ui->log_passwordText->text().toStdString();
    if (!KeyManager::getKey(file_path, passphrase,
                            &this->secret_key_, this->secret_key_len_,
                            &this->public_key_, this->public_key_len_)) {
        qDebug() << "failed to get key";
        return;
    }

    qDebug() << "success get key";
    qDebug() << "sec: " << QString(KeyManager::convertToBase64(reinterpret_cast<unsigned char*>(this->secret_key_), this->secret_key_len_).c_str());
    qDebug() << "public key: " << QString(KeyManager::convertToBase64(reinterpret_cast<unsigned char*> (this->public_key_), this->public_key_len_).c_str());

}
