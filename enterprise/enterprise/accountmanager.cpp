#include "accountmanager.h"
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <QByteArray>

#define SOCKETIO_SERVER "https://192.168.1.167:3000"

AccountManager* AccountManager::getInstance() {
    static AccountManager* instance = NULL;

    if (instance == NULL) {
        instance = new AccountManager();
    }

    return instance;
}

AccountManager::AccountManager(QObject* parent): QObject(parent)
{
    username_ = "";
    password_ = "";

    secret_key_ = NULL;
    secret_key_len_ = 0;

    public_key_ = NULL;
    public_key_len_ = 0;

    // connect to file server
    // change if ip and port number is wrong
    socket_manager = new ZmqManager("localhost", "7777");
    proxy_socket_.connect(SOCKETIO_SERVER);

    proxy_socket_.socket()->on("newDeal", std::bind( &AccountManager::onNewDealReply, this, std::placeholders::_1, std::placeholders::_2));
}

void AccountManager::onNewDealReply(const std::string& mes, sio::message::ptr const& data){
    qDebug() << QString(mes.c_str()) << " " << QString(data->get_string().c_str());
}
AccountManager::~AccountManager() {
    delete socket_manager;
}

void AccountManager::setPassword(std::string password) {
    password_ = password;
}

void AccountManager::setUsername(std::string username) {
    username_ = username;
}

bool AccountManager::authenticate() {
    if (username_ == "" || password_ == "") {
        return false;
    }

    std::string file_path = (QDir::currentPath() + "/keystore/" + QString(username_.c_str()) + ".key").toStdString();
    std::string passphrase = password_;

    if (!KeyManager::getKey(file_path, passphrase,
                            &this->secret_key_, this->secret_key_len_,
                            &this->public_key_, this->public_key_len_,
                            &this->sig_sec_key_, this->sig_sec_key_len_,
                            &this->sig_pub_key_, this->sig_pub_key_len_)) {
        qDebug() << "failed to get key";
        return false;
    }

    qDebug() << "success get key";
    qDebug() << "sec: " << QString(Utils::convertToBase64(reinterpret_cast<unsigned char*>(this->secret_key_), this->secret_key_len_).c_str());
    qDebug() << "public key: " << QString(Utils::convertToBase64(reinterpret_cast<unsigned char*> (this->public_key_), this->public_key_len_).c_str());

    return true;
}

void AccountManager::setSessionPublicKey(std::string& walletID, std::string publickey) {
    // try to connect to socket io server
    while(!proxy_socket_.opened()) {
        proxy_socket_.connect(SOCKETIO_SERVER);
    }

    // send public key
    sio::message::list params;
    params.push(Config::getInstance()->getWalletAddress());
    params.push(std::string(this->public_key_));
    proxy_socket_.socket()->emit("newBidder", params);
}

bool AccountManager::registerNewUser() {
    QString folderContainKeys = QDir::currentPath() + "/keystore";

    // create folder contain key if not exists before
    Utils::createFolder(folderContainKeys.toStdString());

    //if it's ok then create key file
    std::string file_path = (folderContainKeys + "/" + QString(username_.c_str()) + ".key").toStdString();
    std::string passphrase = password_;

    if (!KeyManager::createKey(file_path, passphrase)) {
        return false;
    }

    return true;
}

std::vector<std::string> AccountManager::getAllUserKey() {
    std::vector<std::string> rsa_keys;
    try {
        std::string blockchain_addr = "0xbf011be8b2cee69ef4bd242013377b8945298dde"; // fake
        std::string blockchain_pass = "123"; // fake

        bool check = blockchain_.UnlockAccount(blockchain_addr, blockchain_pass, 30);

        deal_contract_addr_ = Config::getInstance()->getDealContractAddress();
        owner_key_addr_ = Config::getInstance()->getOwnerKeyContractAddress();

        std::string get_all_user_id_param = bitmile::blockchain::OwnerKeyContract::GetAllUserId();
        nlohmann::json result;
        blockchain_.SendCall(blockchain_addr, Config::getInstance()->getOwnerKeyContractAddress(), get_all_user_id_param, "1",  result);

        // parse result
        result = bitmile::blockchain::OwnerKeyContract::ParseGetAllUserId(result);
        std::string rsa_key_data;

        for (nlohmann::json::iterator i = result.begin(); i != result.end(); i++) {
            nlohmann::json get_RSA_PK_result;

            // get RSA public key from user_id
            rsa_key_data = bitmile::blockchain::OwnerKeyContract::GetPubKey(*i);
            blockchain_.SendCall(blockchain_addr, Config::getInstance()->getOwnerKeyContractAddress(), rsa_key_data, "1",  get_RSA_PK_result);

            get_RSA_PK_result = bitmile::blockchain::OwnerKeyContract::ParseGetPubKeyResult(get_RSA_PK_result);
            rsa_keys.push_back(get_RSA_PK_result["key"]);
        }
    }
    catch (std::exception &e) {
        std::cout << "AccountManager::getAllUserKey have error " << e.what() << std::endl;
    }

    return rsa_keys;
}

void AccountManager::clearCredential(){
    username_ = "";
    password_ = "";

    secret_key_ = NULL;
    secret_key_len_ = 0;

    public_key_ = NULL;
    public_key_len_ = 0;
}

void AccountManager::addKeyword(std::string keyword) {
    //TODO: preprocessing keyword

    if (keywords_.find(keyword) != keywords_.end()) {
        return;
    }
    keywords_.insert(keyword);
    Q_EMIT keywords_array_changed();
}

void AccountManager::removeKeyword(std::string keyword) {
    std::set<std::string>::iterator pos = keywords_.find(keyword);

    if (pos != keywords_.end()) {
        keywords_.erase(pos);
        Q_EMIT keywords_array_changed();
    }

}

void AccountManager::clearKeywords() {
    keywords_.clear();
    Q_EMIT keywords_array_changed();
}

std::set<std::string> AccountManager::getKeywords() {
    return keywords_;
}

std::vector<bitmile::db::Document> AccountManager::getSearchedDoc() {
    return searched_docs_;
}
void AccountManager::search () {
    searched_docs_.clear();
    std::vector<std::string> result = this->getAllUserKey();

    // with each public key, encrypt and compare with file server
    std::vector<std::string> keywords;
    std::vector<bitmile::db::Document> searched_docs;

    for (std::vector<std::string>::iterator i = result.begin(); i != result.end(); i++) {
        std::cout << "RSA public key " << *i << std::endl;
        searched_docs.clear();
        keywords.clear();

        BIO *bio = BIO_new_mem_buf(i->data(), i->length());
        RSA *rsa = PEM_read_bio_RSA_PUBKEY(bio, NULL, NULL, NULL);

        for (std::set<std::string>::iterator j = keywords_.begin(); j != keywords_.end(); j++) {
           std::cout << "encrypt for key word " << j->data() << std::endl;

            // convert ansi to utf8
            //QByteArray byteArray = QString::fromStdString(*j).toUtf8();
            std::vector<unsigned char> encrypted(4096);
            std::cout << "size of encrypt data buffer " << encrypted.size();
            //memset(encrypted.data(), 0, encrypted.size());
            std::cout << "size of rsa key " << RSA_size(rsa) << std::endl;

            int encrypt_size = RSA_public_encrypt(RSA_size(rsa)/*byteArray.length()*/, reinterpret_cast<const unsigned char*>(byteArray.data()), reinterpret_cast<unsigned char*>(&encrypted[0]),
                                       rsa, RSA_NO_PADDING);

            std::string encrypt_base64;
            std::cout << "keyword " << *j << " after encrypt length " << encrypt_size << std::endl;
            encrypt_base64 = Utils::convertToBase64(reinterpret_cast<const unsigned char*>(encrypted.data()), 344);
            std::cout << "keyword " << *j << " encrypt and convert to base64 " << encrypt_base64 << std::endl;

            keywords.push_back(encrypt_base64);
        }

        socket_manager->search(keywords, searched_docs);

        for(std::vector<bitmile::db::Document>::iterator k = searched_docs.begin(); k != searched_docs.end(); k++) {
            std::cout << "have result doc searched " << std::endl;
            searched_docs_.push_back(*k);
        }

        // clear openssl context
        delete rsa;
        delete bio;
    }

    if (searched_docs_.size() > 0) {

    }

    Q_EMIT search_done();
}


bool AccountManager::createDeal(std::string blockchain_addr, std::string blockchain_pass, long long prize, QDateTime expiredTime, int &global_id) {
    //qDebug() << "unlock account: " << blockchain_.UnlockAccount(blockchain_addr, blockchain_pass, 10);
    // emit binary
    //char buf[] = "{\"mes\": \"test\"}";
    //proxy_socket_.socket()->emit("newDeal", std::make_shared<std::string>(buf,23));
    try {
        qDebug() << "create deal";

        bool check = blockchain_.UnlockAccount(blockchain_addr, blockchain_pass, 30);

        std::string latest_block;

        deal_contract_addr_ = Config::getInstance()->getDealContractAddress();
        owner_key_addr_ = Config::getInstance()->getOwnerKeyContractAddress();

        std::string transaction_hash;
        if (check) {
            std::string create_deal_param = bitmile::blockchain::DealContract::CreateDeal(prize, expiredTime.toSecsSinceEpoch(), std::string (public_key_, public_key_len_));
            std::cout << "create_deal_param " << create_deal_param << std::endl;

            nlohmann::json result;

            blockchain_.GetBlockNumber("1", result);
            latest_block = result["result"];

            blockchain_.EstimateGas(blockchain_addr, deal_contract_addr_, "0x0", "0x0", create_deal_param, "1", result);

            std::string gas = result["result"];
            blockchain_.SendTransaction(blockchain_addr, deal_contract_addr_, "0x0", gas, create_deal_param, "1", result);

            transaction_hash = result["result"];

            check &= (transaction_hash != "");
        }

        //get dealId
        long long dealId = 0;
        if (check) {
            nlohmann::json result;
            std::vector<std::string> topics;

            std::string topic_hash_str = bitmile::blockchain::DealContract::GetFunctionHash(
                        bitmile::blockchain::DealContract::Functions::LOG_DEAL_CREATED);

            topics.push_back("0x" + Utils::convertToHex(reinterpret_cast<unsigned const char*> (topic_hash_str.c_str()), topic_hash_str.length()));

            int log_count = 0;
            //wait for 1 second before get log

            //sleep (1000);
            int retries = 0;

            while (log_count < 1) {

                if (blockchain_.Createfilter (latest_block, "latest", deal_contract_addr_, topics,"1", result)) {
                    std::string log_id = result["result"];
                    result.clear();
                    blockchain_.GetFilterLogs(log_id, "1", result);

                    nlohmann::json log = bitmile::blockchain::DealContract::ParseLogCreateDeal(result);

                    for (nlohmann::json::iterator it = log.begin(); it != log.end(); it++) {
                        if ((*it)["transactionHash"] == transaction_hash) {
                            dealId = (*it)["data"]["dealId"];
                            global_id = dealId;
                            log_count++;
                        }
                    }
                }
            }

            check &= (log_count == 1);

        }


        //send dealId and user list to proxy server
        if (check) {
            nlohmann::json data;
            data["dealId"] = dealId;
            data["bidder"] = blockchain_addr;

            std::vector<std::string> owner_arr,
                                     doc_id_arr;
            for (int i = 0; i < searched_docs_.size(); i++) {

                std::string owner_addr = searched_docs_[i].GetOwnerAddress();

                //get owner pub key
                std::string get_key = bitmile::blockchain::OwnerKeyContract::GetPubKey(owner_addr);
                nlohmann::json result;

                if (!blockchain_.SendCall(blockchain_addr, owner_key_addr_, get_key, "1", result)) {
                    //failed to get owner key
                    continue;
                }

                nlohmann::json key_json = bitmile::blockchain::OwnerKeyContract::ParseGetPubKeyResult(result);
                //get public key here
                std::string owner_key;
                Utils::convertFromHex(key_json["key"], owner_key);

                //TODO: use owner_key to encrypt doc id
                std::string doc_id_encrypted = searched_docs_[i].GetOwnerDocId();

                owner_arr.push_back(owner_addr);
                doc_id_arr.push_back(doc_id_encrypted);
            }
            data["userIds"] = nlohmann::json (owner_arr);
            data["listEncDocIds"] = nlohmann::json (doc_id_arr);

            std::string mes = data.dump(0);

            char* sig = new char[crypto_sign_BYTES];


            if(crypto_sign_detached(reinterpret_cast<unsigned char*> (sig), NULL,
                                     reinterpret_cast<const unsigned char*> (mes.c_str()), mes.length(),
                                     reinterpret_cast<unsigned char*> (sig_sec_key_)) == 0) {
                nlohmann::json proxy_mes_json;
                //success sign
                proxy_mes_json["data"] = data;
                std::string sig_string (sig, crypto_sign_BYTES);
                proxy_mes_json["signature"] = Utils::convertToHex(reinterpret_cast<const unsigned char*> (sig_string.c_str()),
                                                                  sig_string.length());
                //convert json to std::string
                std::string proxy_mes = proxy_mes_json.dump(0);

                //TODO: send to lists to proxy server
                //proxy_socket_.socket()->emit("newDeal", std::make_shared<std::string>(proxy_mes.c_str(), proxy_mes.length()));
            }else{
                check = false;
            }

            delete [] sig;
        }
        return check;
    } catch (std::exception e) {
        std::cout << "AccountManager::createDeal throw exception " << e.what() << std::endl;
    }
}

QString AccountManager::getSecretKey () const {
    return QString(Utils::convertToBase64(reinterpret_cast<unsigned char*>(this->secret_key_), this->secret_key_len_).c_str());
}


QString AccountManager::getPublicKey () const{
    return QString(Utils::convertToBase64(reinterpret_cast<unsigned char*> (this->public_key_), this->public_key_len_).c_str());
}

QString AccountManager::getUsername() const {
    return QString::fromStdString(username_);
}

QString AccountManager::getPassword() const {
    return QString::fromStdString(password_);
}

