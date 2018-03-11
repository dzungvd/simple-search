#ifndef _MESSAGE_H
#define _MESSAGE_H

#include <vector>
#include <cstdlib>
#include <assert.h>
#include <string>
#include <cstring>
#include "db_interface.h"

namespace bitmile{
namespace msg {

  //message exchange between clients and servers

  enum MessageType {
    KEYWORD_QUERY,
    KEYWORD_QUERY_REPLY,
    UPLOAD_DOC,
    UPLOAD_DOC_REPLY,
    BLANK,
    ERROR,
  };

  class Message {

  public:
    inline MessageType Type() { return type_; }
    virtual void Serialize(std::vector<char>& return_data) = 0;

  protected:
    virtual void Deserialize(const char* dat, size_t size) = 0;
    MessageType type_;
  };

  class KeywordQueryMes : public Message {
  public:
    KeywordQueryMes(MessageType type, const char* dat, size_t size);
    void Serialize (std::vector<char>& return_data);

    std::vector<std::string> GetKeywords();
  protected:
    void Deserialize(const char* dat, size_t size);

  private:
    std::vector<std::string> keywords_;
  };

  class KeywordQueryReplyMes : public Message {
  public:
    KeywordQueryReplyMes(MessageType type, const char* dat, size_t size);
    void Serialize (std::vector<char>& return_data);
    void SetDocuments (std::vector<db::Document>& input_doc);
    std::vector <db::Document> GetDocs ();
  protected:
    void Deserialize(const char* dat, size_t size);
    std::vector<db::Document> docs_;
  };

  class UploadDocMes : public Message {
  public:
    UploadDocMes (MessageType type, const char* dat, size_t size);
    void Serialize (std::vector<char>& return_data);

    const db::Document& GetDoc ();
  protected:
    void Deserialize (const char* dat, size_t size);
    db::Document doc_;
  };

  class UploadDocReplyMes : public Message {
  public:
    UploadDocReplyMes (MessageType type, const char* dat, size_t size);
    void Serialize (std::vector<char>& return_data);
  protected:
    void Deserialize (const char* dat, size_t size);
    int result_code_;
    std::string message_;
  };

  class ErrorMes : public Message {
  public:
    ErrorMes (MessageType type, const char* dat, size_t size);
    void Serialize (std::vector<char>& return_data);
  protected:
    void Deserialize (const char* dat, size_t size);
  private:
    std::string message;
  };

  class BlankMes : public Message {
  public:
    BlankMes (MessageType type, const char* dat, size_t size);
    void Serialize (std::vector<char>& return_data);
  protected:
    void Deserialize (const char* dat, size_t size) {}
  };

  class MessageFactory {
  public:
    Message* CreateMessage (MessageType type, const char* dat, size_t size);

  protected:
    void Deserialize (const char* dat, size_t size);
  };
} //namespace msg
}//namespace bitmile
#endif
