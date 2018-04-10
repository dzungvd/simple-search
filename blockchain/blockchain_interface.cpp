#include "blockchain_interface.h"

namespace bitmile {
  namespace blockchain {
    BlockchainInterface::BlockchainInterface () {
      curl_.add<CURLOPT_URL>("http://localhost");
      curl_.add<CURLOPT_PORT>(8545);
    }

    bool BlockchainInterface::UnlockAccount (std::string account, std::string passphrase, int time) {
      nlohmann::json data;
      data["jsonrpc"] = "2.0";
      data["method"] = "personal_unlockAccount";
      data["params"] = nlohmann::json::array({account, passphrase, time});
      data["id"] = 1;
      std::string data_str = data.dump(0);

      std::string result;
      curl_.add<CURLOPT_POST>(1L);
      curl_.add<CURLOPT_POSTFIELDSIZE> (data_str.length());
      curl_.add<CURLOPT_POSTFIELDS> (data_str.c_str());
      curl_.add<CURLOPT_WRITEFUNCTION> (CurlWriteCallbackStdString);
      curl_.add<CURLOPT_WRITEDATA> (&result);

      try {
          // Execute the request.
          curl_.perform();
      } catch (curl::curl_easy_exception error) {
          // Otherwise we could print the stack like this:
          error.print_traceback();
      }

      nlohmann::json reply  = nlohmann::json::parse(result);
      if (reply.count("result") == 1) {
          if (reply["result"]) {
              return true;
          }
      }
      return false;
    }

    size_t BlockchainInterface::CurlWriteCallbackStdString(void *contents, size_t size, size_t nmemb, void *s) {
        std::string* str = static_cast <std::string*> (s);
        size_t newLength = size*nmemb;
        size_t oldLength = str->size();

        str->resize(oldLength + newLength);

        std::copy((char*)contents,(char*)contents+newLength,str->begin()+oldLength);

        return size*nmemb;
    }
  }
}
