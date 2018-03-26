#include "message_crypto.h"

namespace bitmile{
  MessageCrypto::MessageCrypto (bool with_public_key){
    if (with_public_key) {
      std::ifstream fin ("public_key.key", std::ios::binary);

      if (!fin.is_open()) {
        std::cout << "failed to open public key file" << std::endl;
        assert (fin.is_open());
      }

      fin.seekg (0, fin.end);
      int length = fin.tellg();
      fin.seekg (0, fin.beg);

      server_public_key_ = new char [length];

      fin.read ((char*) (server_public_key_),length);
      fin.close();

      fin.open ("secret_key.key", std::ios::binary);

      if (!fin.is_open()) {
        std::cout << "failed to open secret key file"<< std::endl;
        assert(fin.is_open());
      }

      fin.seekg (0, fin.end);
      length = fin.tellg();
      fin.seekg (0, fin.beg);

      server_secret_key_= new char [length];
      fin.read ((char*) server_secret_key_, length);
      fin.close();
    }
  }

  bool MessageCrypto::DecryptSetKeyMsg (char* ciphertext, size_t ciphertext_len, std::vector<char>& key, std::vector<char>& nonce)  {
    size_t msg_len = ciphertext_len - crypto_box_SEALBYTES;
    char decrypted[msg_len];

    if (crypto_box_seal_open(reinterpret_cast<unsigned char*> (decrypted), reinterpret_cast<unsigned char*> (ciphertext), ciphertext_len,
                             reinterpret_cast<unsigned char*> (server_public_key_), reinterpret_cast<unsigned char*> (server_secret_key_)) != 0) {
      /* message corrupted or not intended for this recipient */
      return false;
    }

    std::string json_str(decrypted, msg_len);
    nlohmann::json json_parsed = nlohmann::json::parse (json_str);

    if (json_parsed.count("key") != 1 ||
        json_parsed.count("nonce") != 1) {
      return false;
    }

    std::string key_b64 = json_parsed["key"];

    key.resize(crypto_aead_xchacha20poly1305_ietf_KEYBYTES + 100);
    size_t key_size;
    if (sodium_base642bin (reinterpret_cast<unsigned char*> (key.data()), crypto_aead_xchacha20poly1305_ietf_KEYBYTES + 100,
                           key_b64.data(), key_b64.length(),
                           NULL, &key_size,
                           NULL, sodium_base64_VARIANT_ORIGINAL)) {
      //failed to decode string
      key = std::vector<char> ();
      return false;
    }

    key.resize (key_size);

    std::string nonce_b64 = json_parsed["nonce"];
    nonce.resize (crypto_aead_xchacha20poly1305_ietf_NPUBBYTES + 100);

    size_t nonce_size;

    if (sodium_base642bin (reinterpret_cast<unsigned char*> (nonce.data()), crypto_aead_xchacha20poly1305_ietf_NPUBBYTES + 100,
                           nonce_b64.data(), nonce_b64.length(),
                           NULL, &nonce_size,
                           NULL, sodium_base64_VARIANT_ORIGINAL)) {
      //failed to decode string
      nonce = std::vector<char> ();
      return false;
    }

    nonce.resize (nonce_size);

    return true;
  }


  bool MessageCrypto::DecryptMsgWithKey (char* dat, size_t size, std::vector<char>& key, std::vector<char>& nonce, std::vector<char>& raw_data){
    raw_data.resize (size);

    unsigned long long decrypted_len = 0;

    if (crypto_aead_xchacha20poly1305_ietf_decrypt(reinterpret_cast<unsigned char*> (raw_data.data()), &decrypted_len,
                                                   NULL,
                                                   reinterpret_cast<unsigned char*>(dat), size,
                                                   NULL,
                                                   0,
                                                   reinterpret_cast<unsigned char*>(nonce.data()), reinterpret_cast<unsigned char*>(key.data())) == 0) {
      raw_data.resize (decrypted_len);
      return true;
    }
    raw_data.resize (0);

    return false;
  }

  bool MessageCrypto::EncryptMsgWithKey (char* dat, size_t size, std::vector<char>& key, std::vector<char>& nonce, std::vector<char>& ciphertext){

    unsigned long long ciphertext_len = 0;
    ciphertext.resize (size + crypto_aead_xchacha20poly1305_ietf_ABYTES);

    crypto_aead_xchacha20poly1305_ietf_encrypt(reinterpret_cast<unsigned char*> (ciphertext.data()), &ciphertext_len,
                                               reinterpret_cast<unsigned char*> (dat), size,
                                               NULL, 0,
                                               NULL, reinterpret_cast<unsigned char*>(nonce.data()), reinterpret_cast<unsigned char*>(key.data()));

    ciphertext.resize (ciphertext_len);
  }

}
