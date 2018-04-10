#ifndef _BLOCKCHAIN_INTERFACE_
#define _BLOCKCHAIN_INTERFACE_

#include "curl_easy.h"
#include "curl_exception.h"
#include <string>
#include "nlohmann/json.hpp"

namespace bitmile {
  namespace blockchain {
    class BlockchainInterface {

    public:
      BlockchainInterface ();
      bool UnlockAccount (std::string account, std::string passphrase, int time);
      static size_t CurlWriteCallbackStdString (void *contents, size_t size, size_t nmemb, void *s);
    private:
      curl::curl_easy curl_;
    };
  }//namespace blockchain
}//namespace bitmile

#endif
