#ifndef MainwindowController_H
#define MainwindowController_H

#include "keymanager.h"
#include <QDir>
#include <QDebug>
#include <string>
#include <QString>

class MainwindowController
{
public:
    MainwindowController();

    //authentication function
    void setUsername (std::string username);
    void setPassword (std::string password_);
    bool authenticate ();
    bool registerNewUser ();
    void clearCredential ();

private:

    //login page attribute
    std::string username_;
    std::string password_;

    //user is identified with two keys
    char* secret_key_;
    char* public_key_;

    size_t secret_key_len_;
    size_t public_key_len_;
};

#endif // MainwindowController_H
