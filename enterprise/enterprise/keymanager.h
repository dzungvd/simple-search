#ifndef KEYMANAGER_H
#define KEYMANAGER_H

#include <iostream>
#include <string>
#include <fstream>
#include "vendors/nlohmann_json/nlohmann/json.hpp"

extern "C" {
#include "sodium.h"
}

class KeyManager
{
public:
    KeyManager();
    static bool getKey (std::string& file_path, std::string& passphrase,
                        char** secret_key, size_t &secret_key_len,
                        char** public_key, size_t &public_key_len);
    static bool createKey (std::string& file_path, std::string& passphrase);
    static std::string convertToBase64 (const unsigned char* input, size_t input_len);
    static std::string convertFromB64ToBin (const char* input, unsigned long long input_len);
};

#endif // KEYMANAGER_H
