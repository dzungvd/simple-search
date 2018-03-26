#ifndef _MESSAGE_CRYPTO_
#define _MESSAGE_CRYPTO_

#include "message.h"
#include <fstream>
#include <assert.h>
#include <vector>

extern "C" {
  #include "sodium.h"
}

namespace bitmile{
  class MessageCrypto {
  public:
    MessageCrypto(bool with_public_key = true);

    bool DecryptSetKeyMsg (char* dat, size_t size, std::vector<char>& key, std::vector<char>& nonce);

    bool DecryptMsgWithKey (char* dat, size_t size, std::vector<char>& key, std::vector<char>& nonce, std::vector<char>& raw_data);

    bool EncryptMsgWithKey (char* dat, size_t size, std::vector<char>& key,  std::vector<char>& nonce, std::vector<char>& encrypted_data);


  private:

    char* server_secret_key_;
    char* server_public_key_;

  };
}//namespace bitmile
#endif
