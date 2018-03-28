#include "keymanager.h"

KeyManager::KeyManager()
{

}

bool KeyManager::getKey(std::string& file_path, std::string& passphrase, char** o_secret_key, char** o_public_key) {

    //read key file and convert it to json format
    std::ifstream key_file (file_path, std::ios::binary);

    if (!key_file.is_open()) {
        return false;
    }

    key_file.seekg(0, key_file.end);
    int length = key_file.tellg();
    key_file.seekg(0, key_file.beg);

    char* buffer = new char[length];

    key_file.read (buffer, length);

    key_file.close();

    nlohmann::json key_json = nlohmann::json::parse(std::string(buffer, length));

    delete [] buffer;

    if (key_json.count("nonce") != 1 ||
            key_json.count ("public_key") != 1 ||
            key_json.count ("ciphertext") != 1 ||
            key_json.count ("salt") != 1) {
        return false;
    }

    //get key info from json object
    std::string nonce_b64 = key_json["nonce"],
            public_key_b64 = key_json["public_key"],
            ciphertext_b64 = key_json["ciphertext"],
            salt_b64 = key_json["salt"];


    std::string nonce = convertFromB64ToBin(nonce_b64.c_str(), nonce_b64.length());
    std::string public_key = convertFromB64ToBin(public_key_b64.c_str(), public_key_b64.length());
    std::string ciphertext = convertFromB64ToBin(ciphertext_b64.c_str(), ciphertext_b64.length());
    std::string salt = convertFromB64ToBin(salt_b64.c_str(), salt_b64.length());

    if (nonce == "" || public_key == "" || ciphertext == "" || salt == "") {
        //failed to decode string
        return false;
    }

    //regenerate key from salt and passphrase
    if (salt.length() != crypto_pwhash_SALTBYTES) {
        std::cout << "salt size differ: " << salt.length() << " "
                  << " should be: " << crypto_pwhash_SALTBYTES << std::endl;
        return false;
    }

    unsigned char key[crypto_aead_xchacha20poly1305_ietf_KEYBYTES];

    if (crypto_pwhash(key, sizeof key, passphrase.c_str(),
                      passphrase.length(), reinterpret_cast<const unsigned char*>(salt.c_str()),
                      crypto_pwhash_OPSLIMIT_INTERACTIVE, crypto_pwhash_MEMLIMIT_INTERACTIVE,
                      crypto_pwhash_ALG_DEFAULT) != 0) {
        /* some error */
        return false;
    }

    //get secret key
    size_t MESSAGE_LEN = ciphertext.length();
    unsigned char decrypted[MESSAGE_LEN];
    unsigned long long decrypted_len = 0;
    if (crypto_aead_xchacha20poly1305_ietf_decrypt(decrypted, &decrypted_len,
                                                   NULL,
                                                   reinterpret_cast <const unsigned char*> (ciphertext.c_str()),
                                                   ciphertext.length(),
                                                   NULL,
                                                   0,
                                                   reinterpret_cast<const unsigned char*> (nonce.c_str()), key) != 0) {
        /* message forged! */
        return false;
    }

    if (decrypted_len != crypto_box_SECRETKEYBYTES ||
            public_key.length() != crypto_box_PUBLICKEYBYTES) {
        //key length is invalid
        return false;
    }

    (*o_secret_key) = new char [decrypted_len];
    memcpy((*o_secret_key), decrypted, decrypted_len);

    (*o_public_key) = new char [public_key.length()];
    memcpy((*o_public_key), public_key.c_str(), public_key.length());

    return true;
}

bool KeyManager::createKey(std::string& file_path, std::string& passphrase) {

    //open file first, if it's failed then no need to prepare the data

    std::ofstream key_file (file_path);

    if (!key_file.is_open()) {
        return false;
    }


    //prepare key file data
    /*
     * {
     *   "nonce" : ""
     *   "ciphertext" : ""
     *   "salt" : ""
     *   "public_key" : ""
     * }
     *
     * "salt" + passphrase (from user) -> generate secret "key"
     *
     * "key" then be used with "nonce" to decrypt "ciphertext" => "private key"
     *
     * "private key" + "public key" is used to identify user
     */

    unsigned char pk [crypto_box_PUBLICKEYBYTES];
    unsigned char sk [crypto_box_SECRETKEYBYTES];

    crypto_box_keypair(pk, sk);

    nlohmann::json key_json;
    //save public key to json
    key_json["public_key"] = convertToBase64(pk, sizeof pk);

    unsigned char key[crypto_aead_xchacha20poly1305_ietf_KEYBYTES];
    unsigned char nonce[crypto_aead_xchacha20poly1305_ietf_NPUBBYTES];

    //generate nonce
    randombytes_buf(nonce, sizeof nonce);

    //convert nonce to base 64 and save to json
    key_json["nonce"] = convertToBase64(nonce, sizeof nonce);

    //generate key
    unsigned char salt[crypto_pwhash_SALTBYTES];
    randombytes_buf(salt, sizeof salt);

    //save salt to key file
    key_json["salt"] = convertToBase64(salt, sizeof salt);

    if (crypto_pwhash(key, sizeof key, passphrase.c_str(), passphrase.length(), salt,
                      crypto_pwhash_OPSLIMIT_INTERACTIVE, crypto_pwhash_MEMLIMIT_INTERACTIVE,
                      crypto_pwhash_ALG_DEFAULT) != 0) {
        /* some error */
        return false;
    }


    //encrypt secret key with generated key
    unsigned char ciphertext[sizeof sk + crypto_aead_xchacha20poly1305_ietf_ABYTES];
    unsigned long long ciphertext_len;

    crypto_aead_xchacha20poly1305_ietf_encrypt(ciphertext, &ciphertext_len,
                                               sk, sizeof sk,
                                               NULL, 0,
                                               NULL, nonce, key);

    //save private key to json file
    key_json["ciphertext"] = convertToBase64(ciphertext, sizeof ciphertext);


    key_file << key_json.dump();

    return true;
}

std::string KeyManager::convertToBase64(const unsigned char *input, size_t input_len) {

    if (input == NULL) {
        return std::string ("");
    }

    size_t b64_maxlen = sodium_base64_ENCODED_LEN(input_len, sodium_base64_VARIANT_ORIGINAL);
    //encode len = string_length + '\0' character

    char* b64 = new char [b64_maxlen];

    sodium_bin2base64(b64, b64_maxlen,
                      input, input_len,
                      sodium_base64_VARIANT_ORIGINAL);

    //b64_maxlen is string length plus '\0' char => discard the '\0' char before
    //puting it to result string
    std::string result (b64, b64_maxlen - 1);

    delete[] b64;
    return result;
}

std::string KeyManager::convertFromB64ToBin(const char *input, unsigned long long input_len) {
    if (input == NULL) {
        return std::string ("");
    }
    size_t bin_maxlen = input_len * 2;
    char* bin = new char [bin_maxlen];
    size_t bin_len = 0;
    if (sodium_base642bin(reinterpret_cast<unsigned char*> (bin), bin_maxlen,
                          input, input_len,
                          NULL, &bin_len,
                          NULL, sodium_base64_VARIANT_ORIGINAL) != 0) {
        delete[] bin;
        return std::string ("");
    }
    std::string result (bin, bin_len);
    delete[] bin;
    return result;
}
