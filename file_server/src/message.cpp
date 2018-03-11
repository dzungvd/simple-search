#include "message.h"

namespace bitmile {
  namespace msg {

    KeywordQueryMes::KeywordQueryMes (MessageType type, const char* dat, size_t size) {
      type_ = type;
      Deserialize (dat, size);
    }

    void KeywordQueryMes::Deserialize(const char* dat, size_t size) {
      /*
       * Parse binary data
       */
      if (size <= 1) {
        std::cout << "invalid querry" << std::endl;
        //invalid string since one string must contain
        //at least one charater and a NULL character
        //no need to parse
        return;
      }

      if (dat[size-1] != '\0') {
        std::cout << "last character is not null" << std::endl;
        //the last character should be a terminate NULL
        //or else it's an invalid message
        return;
      }

      //TODO: handle case "continuous NULL character"
      int offset = 0;
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
      offset += sizeof (MessageType);

      for (int i = 0; i < keywords_.size(); i++) {
        memcpy (&return_data[offset], keywords_[i].c_str(), keywords_[i].length() + 1);
        offset += keywords_[i].length() + 1;

      }

    }

    std::vector<std::string> KeywordQueryMes::GetKeywords() {
      return keywords_;
    }
    KeywordQueryReplyMes::KeywordQueryReplyMes (MessageType type, const char* dat, size_t size) {
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
      offset += sizeof (MessageType);

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
      if (size < 1) {
        //invalid string
        return;
      }

      if (dat[size - 1] != '\0'){
        std::cout << "string does not contain NULL terminate" << std::endl;
        //query string must contain NULL character at the end
        return;
      }

      int offset = 0;
      std::string parsed_word;
      int parsed_word_len = 0;
      db::Document temp_doc;
      while (offset < size) {

        parsed_word = std::string (dat);
        //length is word length + 1 NULL char
        parsed_word_len = parsed_word.length() + 1;
        temp_doc.SetOwnerAddress (parsed_word);
        if (offset >= size) return;
        dat += parsed_word_len;
        offset += parsed_word_len;

        parsed_word = std::string (dat);
        //length is word length + 1 NULL char
        parsed_word_len = parsed_word.length() + 1;
        temp_doc.SetOwnerDocId (parsed_word);
        if (offset >= size) return;
        dat += parsed_word_len;
        offset += parsed_word_len;

        parsed_word = std::string (dat);
        //length is word length + 1 NULL char
        parsed_word_len = parsed_word.length() + 1;
        temp_doc.SetElasticDocId (parsed_word);
        if (offset >= size) return;
        dat += parsed_word_len;
        offset += parsed_word_len;

        docs_.push_back (temp_doc);
      }
    }

    void KeywordQueryReplyMes::SetDocuments (std::vector<db::Document>& input_doc) {
      docs_ = std::vector <db::Document> (input_doc);
    }

    std::vector<db::Document> KeywordQueryReplyMes::GetDocs () {
      return docs_;
    }

    UploadDocMes::UploadDocMes (MessageType type, const char* dat, size_t size) {
      type_ = type;
      Deserialize (dat, size);
    }

    void UploadDocMes::Serialize (std::vector<char>& return_data) {
      nlohmann::json json_body = doc_.ToJson();
      std::string json_body_str = json_body.dump();

      //add extra byte for teminate NULL byte
      return_data.reserve (sizeof (MessageType) + json_body_str.length() + 1);
      size_t offset = 0;
      return_data.resize (sizeof (MessageType));
      memcpy (return_data.data(), &type_, sizeof (MessageType));
      offset += sizeof (MessageType);
      if (json_body_str.length() > 0) {
        return_data.resize ( return_data.size() + json_body_str.length() + 1);
        memcpy (return_data.data() + offset, json_body_str.c_str(), json_body_str.length() + 1);
      }

    }

    void UploadDocMes::Deserialize (const char* dat, size_t size) {

      //json body should have length > 1
      if (size <= 1) {
        return;
      }

      //last byte should be terminate byte
      if (dat[size - 1] != '\0')  {
        return;
      }

      std::string json_body (dat);
      nlohmann::json json_parsed = nlohmann::json::parse (json_body);

      doc_.FromJson (json_parsed);

    }

    const db::Document& UploadDocMes::GetDoc () {
      return doc_;
    }

    UploadDocReplyMes::UploadDocReplyMes (MessageType type, const char* dat, size_t size) {
      type_ = type;
      Deserialize (dat, size);
    }

    void UploadDocReplyMes::Serialize (std::vector<char>& return_data) {
      /*
       * message:
       * -------------------
       *|result| message   |
       *-------------------
       * result: int type (4 bytes)
       * message: string type
       */

      return_data.reserve (sizeof (MessageType) + sizeof (int) + message_.length() + 1);
      return_data.resize (sizeof (MessageType));

      memcpy (return_data.data(), &type_, sizeof (MessageType));

      int offset = sizeof(MessageType);

      return_data.resize (sizeof (MessageType) + sizeof (int));

      memcpy (return_data.data() + offset, &result_code_, sizeof (int));

      offset += sizeof (int);

      if (message_.length() > 0) {
        return_data.resize (message_.length() + 1);
        memcpy (return_data.data() + offset, message_.c_str(), message_.length() + 1);
      }

    }

    void UploadDocReplyMes::Deserialize (const char* dat, size_t size) {

      if (size < sizeof (int))  {
        return;
      }

      memcpy (&result_code_, dat, sizeof(int));
      int offset = sizeof (int);

      if (size > offset) {
        message_ = std::string (dat+ offset);
      }
    }

    ErrorMes::ErrorMes (MessageType type, const char* dat, size_t size){
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


    BlankMes::BlankMes (MessageType type, const char* dat, size_t size) {
      type = type;
      Deserialize(dat, size);
    }

    void BlankMes::Serialize (std::vector<char>& return_data) {
      return_data.resize (sizeof (MessageType));
      memcpy (return_data.data(), &type_, sizeof (MessageType));
    }

    Message* MessageFactory::CreateMessage (MessageType type, const char* dat, size_t size) {

      switch (type) {
      case MessageType::KEYWORD_QUERY:
        return new KeywordQueryMes (type, dat, size);
      case MessageType::KEYWORD_QUERY_REPLY:
        return new KeywordQueryReplyMes (type, dat, size);
      case MessageType::UPLOAD_DOC:
        return new UploadDocMes (type, dat, size);
      case MessageType::UPLOAD_DOC_REPLY:
        return new UploadDocReplyMes (type, dat, size);
      case MessageType::BLANK:
        return new BlankMes (type, dat, size);

      default:
        return new ErrorMes (MessageType::ERROR, dat, size);
      }
    }
  }
}
