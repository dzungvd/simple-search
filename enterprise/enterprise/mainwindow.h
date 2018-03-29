#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDir>
#include <QDebug>

#include "mainwindow_controller.h"

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

    void on_deal_logout_clicked();

private:
    Ui::MainWindow *ui;
    MainwindowController main_controller_;
};

#endif // MAINWINDOW_H
