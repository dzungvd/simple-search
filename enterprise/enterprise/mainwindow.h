#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDir>
#include <QDebug>
#include <QListWidgetItem>
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
private Q_SLOTS:

    //register page slot
    void on_reg_registerButton_clicked();

    void on_reg_LoginButton_clicked();

    //login page slot
    void on_log_registerButton_clicked();

    void on_log_loginButton_clicked();

    //setting page slot
    void on_deal_logout_clicked();

    //new deal page slot
    void on_new_addKeyword_returnPressed();

    void on_new_keyword_changed ();

    void on_new_keywordList_itemDoubleClicked(QListWidgetItem *item);

    void on_new_searchButton_clicked();

    void on_search_done();

    void on_new_createDealButton_clicked();

private:
    Ui::MainWindow *ui;
    MainwindowController main_controller_;
};

#endif // MAINWINDOW_H
