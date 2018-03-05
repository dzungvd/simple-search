#include "message.h"

namespace bitmile {
  namespace msg {

    KeywordQueryMes::KeywordQueryMes (MessageType type, char* dat, size_t size) {
      type_ = type;
      Deserialize (dat, size);
    }

    void KeywordQueryMes::Deserialize(const char* dat, size_t size) {
      /*
       * Parse binary data
       */
      if (size < sizeof (MessageType)) {
        return;
      }

      MessageType type;
      memcpy (&type, dat, sizeof (MessageType));

      dat += sizeof (MessageType);

      int offset = sizeof (MessageType);

      int keyword_len = 0;

      if (size <= 1) {
        //invalid string since one string must contain
        //at least one charater and a NULL character
        //no need to parse
        return;
      }

      if (dat[size - 1 - sizeof (MessageType)] != '\0') {
        //the last character should be a terminate NULL
        //or else it's an invalid message
        return;
      }

      //TODO: handle case "continuous NULL character"
      while (offset < size) {
        keywords_.push_back (std::string (dat));
        keyword_len = keywords_.back().length() + 1;
        dat += keyword_len;
        offset += keyword_len;
      }
    }

    void KeywordQueryMes::Serialize (std::vector<char>& return_data) {
      /*
       * Get data binary representation
       * Keyword message:
       * ------------------------
       *|type|key1|key2|key3|key4|
       *-------------------------
       * type is message type (int type) - 4 bytes
       * keyx is keyword x (string type) - seperate by NULL character
       */
      //TODO: add support for unicode here
      //Get data size

      int size = sizeof (MessageType);

      for (int i = 0; i < keywords_.size(); i++) {
        //1 additional byte for NULL charactrer
        size += keywords_[i].length() + 1;
      }

      return_data.resize (size);

      //copy data to container
      int offset = 0;

      memcpy (&return_data[offset], &type_, sizeof (MessageType));

      for (int i = 0; i < keywords_.size(); i++) {
        memcpy (&return_data[offset], keywords_[i].c_str(), keywords_[i].length() + 1);
        offset += keywords_[i].length() + 1;

      }

    }

    std::vector<std::string> KeywordQueryMes::GetKeywords() {
      return keywords_;
    }
    KeywordQueryReplyMes::KeywordQueryReplyMes (MessageType type, char* dat, size_t size) {
      type_ = type;
      Deserialize (dat, size);
    }

    void KeywordQueryReplyMes::Serialize (std::vector<char>& return_data) {
      return_data.resize (sizeof (MessageType));
      memcpy (return_data.data(), &type_, sizeof (MessageType));
    }
    void KeywordQueryReplyMes::Deserialize (const char* dat, size_t size) {

    }
    ErrorMes::ErrorMes (MessageType type, char* dat, size_t size){
      type_ = type;
      Deserialize (dat, size);
    }

    void ErrorMes::Deserialize (const char* dat, size_t size) {
      return;
    }

    void ErrorMes::Serialize (std::vector<char>& return_data) {
      return_data.resize (sizeof (MessageType));
      memcpy (return_data.data(), &type_, sizeof (MessageType));
    }


    BlankMes::BlankMes (MessageType type, char* dat, size_t size) {
      type = type;
      Deserialize(dat, size);
    }

    void BlankMes::Serialize (std::vector<char>& return_data) {
      return_data.resize (sizeof (MessageType));
      memcpy (return_data.data(), &type_, sizeof (MessageType));
    }

    Message* MessageFactory::CreateMessage (MessageType type, char* dat, size_t size) {

      switch (type) {
      case MessageType::KEYWORD_QUERY:
        return new KeywordQueryMes (type, dat, size);
      case MessageType::KEYWORD_QUERY_REPLY:
        return new KeywordQueryReplyMes (type, dat, size);
      case MessageType::BLANK:
        return new BlankMes (type, dat, size);

      default:
        return new ErrorMes (MessageType::ERROR, dat, size);
      }
    }
  }
}
