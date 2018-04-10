#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentWidget(ui->LoginPage);
    ui->new_keywordList->hide();
    ui->new_searchResultFrame->hide();

    //connect slots
    connect(&main_controller_, SIGNAL(keywords_array_changed()),
            this, SLOT(on_new_keyword_changed()));

    connect(&main_controller_, SIGNAL(search_done()),
            this, SLOT(on_search_done()));
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

void MainWindow::on_new_addKeyword_returnPressed()
{
    if (ui->new_addKeyword->text() != "") {
        main_controller_.addKeyword(ui->new_addKeyword->text().toLower().toStdString());
    }
    ui->new_addKeyword->clear();
}

void MainWindow::on_new_keyword_changed() {
    std::set<std::string> keywords = main_controller_.getKeywords();
    while (ui->new_keywordList->count() > 0) {
        delete ui->new_keywordList->takeItem(0);
    }
    if (keywords.size() == 0) {
        ui->new_keywordList->hide();
    }

    for (std::set <std::string>::iterator beg = keywords.begin();
         beg != keywords.end(); beg++) {
        ui->new_keywordList->addItem(QString( (*beg).c_str() ));
    }

    ui->new_keywordList->show();
}

void MainWindow::on_new_keywordList_itemDoubleClicked(QListWidgetItem *item)
{
    //update controller data
    main_controller_.removeKeyword(item->text().toStdString());

}

void MainWindow::on_new_searchButton_clicked()
{
    main_controller_.search ();
}

void MainWindow::on_search_done() {
    while (ui->new_documentList->count() > 0) {
        delete ui->new_documentList->takeItem(0);
    }

    std::vector<bitmile::db::Document> docs = main_controller_.getSearchedDoc();

    for (int i = 0; i < docs.size(); i++) {
        ui->new_documentList->addItem(QString (docs[i].GetElasticDocId().c_str()));
    }
    ui->new_searchResultFrame->show();
}


void MainWindow::on_new_createDealButton_clicked()
{
    //create new deal, publish it to blockchain
    if (ui->new_dealPrize->text() == "") {

    }

    if (ui->new_blockchainAddr->text() == "") {

    }

    if (ui->new_blockchainPass->text() == "") {

    }
    main_controller_.createDeal(ui->new_blockchainAddr->text().toStdString(),
                                ui->new_blockchainPass->text().toStdString());

}
