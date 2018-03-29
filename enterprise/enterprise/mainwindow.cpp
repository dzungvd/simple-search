#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
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

    main_controller_.setPassword(ui->reg_passwordText->text().toStdString());
    main_controller_.setUsername(ui->reg_usernameText->text().toStdString());
    flag &= main_controller_.registerNewUser();


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

    main_controller_.setPassword(ui->log_passwordText->text().toStdString());
    main_controller_.setUsername(ui->log_usernameText->text().toStdString());

    if (!main_controller_.authenticate()) {
        return;
    }

    ui->stackedWidget->setCurrentWidget(ui->DealManagerPage);
}

void MainWindow::on_deal_logout_clicked()
{
    main_controller_.clearCredential();
    ui->stackedWidget->setCurrentWidget(ui->LoginPage);
}
