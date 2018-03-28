#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDir>
#include <QDebug>

#include "keymanager.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
private slots:

    void on_reg_registerButton_clicked();

    void on_reg_LoginButton_clicked();

    void on_log_registerButton_clicked();

    void on_log_loginButton_clicked();

private:
    Ui::MainWindow *ui;

    //user is identified with two keys
    char* secret_key_;
    char* public_key_;

    size_t secret_key_len_;
    size_t public_key_len_;
};

#endif // MAINWINDOW_H
