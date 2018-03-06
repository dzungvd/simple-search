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

      int offset = 0;


      if (size <= 1) {
        //invalid string since one string must contain
        //at least one charater and a NULL character
        //no need to parse
        return;
      }

      if (dat[size] != '\0') {
        //the last character should be a terminate NULL
        //or else it's an invalid message
        return;
      }

      //TODO: handle case "continuous NULL character"
      int keyword_len = 0;
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
      /*
       * reply query mes:
       * ----------------------------------------
       *|type|owner_address_1|doc_id_1|elastic_doc_id_1|....
       * ----------------------------------------
       * type - int - 4 bytes
       * owner_address - string - owner blockchain address
       * doc_id - string - user document id in user database
       * elastic_doc_id - string - elastic system doc id
       * strings are seperated by NULL char
       */

      int size = sizeof (MessageType);
      for (int i = 0; i < docs_.size() ; i++) {
        size += docs_[i].GetOwnerAddress().length() + 1;
        size += docs_[i].GetOwnerDocId().length() + 1;
        size += docs_[i].GetElasticDocId().length() + 1;
      }

      return_data.resize (size);
      int offset = 0;
      memcpy (&return_data[offset], &type_, sizeof (MessageType));

      std::string owner_addr, owner_doc_id, elastic_id;
      for (int i = 0; i < docs_.size() ; i++) {
        owner_addr = docs_[i].GetOwnerAddress();
        owner_doc_id = docs_[i].GetOwnerDocId();
        elastic_id = docs_[i].GetElasticDocId();

        memcpy (&return_data[offset], owner_addr.c_str(), owner_addr.length() + 1);
        offset += owner_addr.length() + 1;
        memcpy (&return_data[offset], owner_doc_id.c_str(), owner_doc_id.length() + 1);
        offset += owner_doc_id.length() + 1;
        memcpy (&return_data[offset], elastic_id.c_str(), elastic_id.length() + 1);
        offset += elastic_id.length() + 1;
      }

    }
    void KeywordQueryReplyMes::Deserialize (const char* dat, size_t size) {

    }

    void KeywordQueryReplyMes::SetDocuments (std::vector<db::Document>& input_doc) {
      docs_ = std::vector <db::Document> (input_doc);
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
