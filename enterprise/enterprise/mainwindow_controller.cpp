#include "mainwindow_controller.h"
#include "internaldb.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <ui_mainwindow.h>>

MainwindowController::MainwindowController()
{
    username_ = "";
    password_ = "";

    secret_key_ = NULL;
    secret_key_len_ = 0;

    public_key_ = NULL;
    public_key_len_ = 0;

    socket_manager = new ZmqManager("localhost", "5555");
    proxy_socket_.connect("https://192.168.1.167:3000");

    proxy_socket_.socket()->on("newDeal", std::bind( &MainwindowController::onNewDealReply, this, std::placeholders::_1, std::placeholders::_2));


}

void MainwindowController::onNewDealReply(const std::string& mes, sio::message::ptr const& data){
    qDebug() << QString(mes.c_str()) << " " << QString(data->get_string().c_str());
}
MainwindowController::~MainwindowController() {
    delete socket_manager;
}

void MainwindowController::setPassword(std::string password) {
    password_ = password;
}

void MainwindowController::setUsername(std::string username) {
    username_ = username;
}

bool MainwindowController::authenticate() {
    if (username_ == "" || password_ == "") {
        return false;
    }

    std::string file_path = (QDir::currentPath() + "/keystore/" + QString(username_.c_str()) + ".key").toStdString();
    std::string passphrase = password_;

    if (!KeyManager::getKey(file_path, passphrase,
                            &this->secret_key_, this->secret_key_len_,
                            &this->public_key_, this->public_key_len_)) {
        qDebug() << "failed to get key";
        return false;
    }

    qDebug() << "success get key";
    qDebug() << "sec: " << QString(Utils::convertToBase64(reinterpret_cast<unsigned char*>(this->secret_key_), this->secret_key_len_).c_str());
    qDebug() << "public key: " << QString(Utils::convertToBase64(reinterpret_cast<unsigned char*> (this->public_key_), this->public_key_len_).c_str());

    /* establish connection to internal DB intergrated with account
     * use username for name of db, confirm for change ???
     */
    InternalDB* db = InternalDB::getInstance();
    db->setSqlDbName(QString::fromStdString(username_));
    db->establiseConnection();

    return true;
}

bool MainwindowController::registerNewUser() {
    //if it's ok then create key file
    std::string file_path = (QDir::currentPath() + "/keystore/" + QString(username_.c_str()) + ".key").toStdString();
    std::string passphrase = password_;

    if (!KeyManager::createKey(file_path, passphrase)) {
        return false;
    }

    return true;
}
void MainwindowController::clearCredential(){
    username_ = "";
    password_ = "";

    secret_key_ = NULL;
    secret_key_len_ = 0;

    public_key_ = NULL;
    public_key_len_ = 0;
    InternalDB::getInstance()->disconnect();
}

void MainwindowController::addKeyword(std::string keyword) {
    //TODO: preprocessing keyword

    if (keywords_.find(keyword) != keywords_.end()) {
        return;
    }
    keywords_.insert(keyword);
    Q_EMIT keywords_array_changed();
}

void MainwindowController::removeKeyword(std::string keyword) {
    std::set<std::string>::iterator pos = keywords_.find(keyword);

    if (pos != keywords_.end()) {
        keywords_.erase(pos);
        Q_EMIT keywords_array_changed();
    }

}

std::set<std::string> MainwindowController::getKeywords() {
    return keywords_;
}

std::vector<bitmile::db::Document> MainwindowController::getSearchedDoc() {
    return searched_docs_;
}
void MainwindowController::search () {
    searched_docs_.clear();
    std::vector<std::string> keywords (this->keywords_.begin(), this->keywords_.end());
    socket_manager->search(keywords, searched_docs_);

    // insert owner address to internal db
    InternalDB* db = InternalDB::getInstance();
    InternalDB::Owner owner;

    if (searched_docs_.size() > 0) {

    }

    Q_EMIT search_done();
}


bool MainwindowController::createDeal(std::string blockchain_addr, std::string blockchain_pass) {
    qDebug() << "unlock account: " << blockchain_.UnlockAccount(blockchain_addr, blockchain_pass, 10);
    // emit binary
    char buf[] = "{\"mes\": \"anh Duc dien\"}";
    proxy_socket_.socket()->emit("newDeal", std::make_shared<std::string>(buf,23));
    return insertToInternalDB();
}

bool MainwindowController::insertToInternalDB() {
    // if searched doc is empty , not continue process
    if (searched_docs_.size() == 0)
        return true;

    // insert deal data to db
    InternalDB::Deal deal;

    deal.price = ui->new_dealPrize->text().toDouble();
    deal.private_key = QString(Utils::convertToBase64(reinterpret_cast<unsigned char*>(this->secret_key_), this->secret_key_len_).c_str());
    deal.public_key =  QString(Utils::convertToBase64(reinterpret_cast<unsigned char*> (this->public_key_), this->public_key_len_).c_str());

    // set current timesamp in local device, confirm for change ???
    deal.time = QDateTime::currentMSecsSinceEpoch();

    // create continuous string from keywords
    QJsonArray listKeyword;
    for (std::set<std::string>::iterator i = keywords_.begin(); i != keywords_.end() ; i++) {
        listKeyword.append(QJsonValue(QString::fromStdString(*i)));
    }

    QJsonObject jsonObj;
    jsonObj["keywords"] = listKeyword;

    deal.keywords = QJsonDocument(jsonObj).toJson(QJsonDocument::Compact);
    qDebug() << "MainWindow Controller deal.keywords " << deal.keywords;

    if (!InternalDB::getInstance()->insertDealData(deal)) {
        return false;
    }


    InternalDB::Owner owner;
    InternalDB::DealOwner dealOwner;

    for (std::vector<bitmile::db::Document>::iterator i = searched_docs_.begin(); i != searched_docs_.end(); i++) {
        owner.address = QString::fromStdString((*i).GetOwnerAddress());
        qDebug() << owner.address;
        if (!InternalDB::getInstance()->insertOwnerData(owner))
            return false;

        dealOwner.deal_time = deal.time;
        dealOwner.owner_address = owner.address;
        dealOwner.status = DEALOWNER_STATUS_WAITING;
        dealOwner.encrypt_data = QString("");
        dealOwner.decrypt_data = QString("");
        dealOwner.owner_secret_key = QString("");
        dealOwner.owner_doc_id = QString::fromStdString((*i).GetOwnerDocId()).toULongLong();
        dealOwner.elastic_id = QString::fromStdString((*i).GetElasticDocId()).toULongLong();
        if (!InternalDB::getInstance()->insertDealOwnerData(dealOwner))
            return false;
    }

    return true;
}

void MainwindowController::setMainWindowPtr (Ui::MainWindow* _ui) {
    ui = _ui;
}
