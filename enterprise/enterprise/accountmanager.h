#ifndef MainwindowController_H
#define MainwindowController_H

#include "keymanager.h"
#include "database/db_interface.h"
#include "message/message.h"
#include "zmq_manager.h"
#include "utils/utils.h"
#include "blockchain/blockchain_interface.h"

#include <sio_client.h>
#include <QDir>
#include <QDebug>
#include <QString>
#include <QObject>

#include <string>
#include <vector>
#include <set>
#include <functional>

class AccountManager : public QObject
{
    Q_OBJECT

public:
    AccountManager(QObject* parent = 0);

    static AccountManager* getInstance();
    //authentication function
    void setUsername (std::string username);
    void setPassword (std::string password_);
    bool authenticate ();
    bool registerNewUser ();
    void clearCredential ();

    //new deal function
    void addKeyword (std::string keyword);
    void removeKeyword (std::string keyword);
    void clearKeywords();
    std::set<std::string> getKeywords();
    std::vector<bitmile::db::Document> getSearchedDoc();
    void search ();

    void onNewDealReply(const std::string& mes, sio::message::ptr const& data);

    bool createDeal(std::string blockchain_addr, std::string blockchain_pass);

    QString getSecretKey () const;
    QString getPublicKey () const;

    QString getUsername() const;
    QString getPassword() const;
    ~AccountManager();

Q_SIGNALS:
    void keywords_array_changed ();
    void search_done ();

private:

    ZmqManager* socket_manager;

    //login page attribute
    std::string username_;
    std::string password_;

    //user is identified with two keys
    char* secret_key_;
    char* public_key_;

    size_t secret_key_len_;
    size_t public_key_len_;

    //new deal atributes
    std::set<std::string> keywords_;
    std::vector<bitmile::db::Document> searched_docs_;

    bitmile::blockchain::BlockchainInterface blockchain_;

    //socket for communicate with proxy server
    sio::client proxy_socket_;
};

#endif // MainwindowController_H
