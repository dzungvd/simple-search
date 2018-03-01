#include "message.h"

namespace bitmile {
  namespace msg {

    KeywordQueryMes::KeywordQueryMes (MessageType type, char* dat, size_t size) {
      type_ = type;
      Deserialize (dat, size);
    }

    void KeywordQueryMes::Deserialize(char* dat, size_t size) {

    }

    void KeywordQueryMes::Serialize (std::vector<char>& return_data) {
      return_data.resize (sizeof (MessageType));
      memcpy (return_data.data(), &type_, sizeof (MessageType));
    }

    KeywordQueryReplyMes::KeywordQueryReplyMes (MessageType type, char* dat, size_t size) {
      type_ = type;
      Deserialize (dat, size);
    }

    void KeywordQueryReplyMes::Serialize (std::vector<char>& return_data) {
      return_data.resize (sizeof (MessageType));
      memcpy (return_data.data(), &type_, sizeof (MessageType));
    }
    void KeywordQueryReplyMes::Deserialize (char* dat, size_t size) {

    }
    ErrorMes::ErrorMes (MessageType type, char* dat, size_t size){
      type_ = type;
      Deserialize (dat, size);
    }

    void ErrorMes::Deserialize (char* dat, size_t size) {
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
