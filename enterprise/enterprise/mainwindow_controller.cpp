#include "mainwindow_controller.h"

MainwindowController::MainwindowController()
{
    username_ = "";
    password_ = "";

    secret_key_ = NULL;
    secret_key_len_ = 0;

    public_key_ = NULL;
    public_key_len_ = 0;

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
    qDebug() << "sec: " << QString(KeyManager::convertToBase64(reinterpret_cast<unsigned char*>(this->secret_key_), this->secret_key_len_).c_str());
    qDebug() << "public key: " << QString(KeyManager::convertToBase64(reinterpret_cast<unsigned char*> (this->public_key_), this->public_key_len_).c_str());

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
}
