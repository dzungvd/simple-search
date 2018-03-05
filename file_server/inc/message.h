#ifndef _MESSAGE_H
#define _MESSAGE_H

#include <vector>
#include <cstdlib>
#include <assert.h>
#include <string>
#include <cstring>

namespace bitmile{
namespace msg {

  //message exchange between clients and servers

  enum MessageType {
    KEYWORD_QUERY,
    KEYWORD_QUERY_REPLY,
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
    KeywordQueryMes(MessageType type, char* dat, size_t size);
    void Serialize (std::vector<char>& return_data);

    std::vector<std::string> GetKeywords();
  protected:
    void Deserialize(const char* dat, size_t size);

  private:
    std::vector<std::string> keywords_;
  };

  class KeywordQueryReplyMes : public Message {
  public:
    KeywordQueryReplyMes(MessageType type, char* dat, size_t size);
    void Serialize (std::vector<char>& return_data);
  protected:
    void Deserialize(const char* dat, size_t size);

  };

  class ErrorMes : public Message {
  public:
    ErrorMes (MessageType type, char* dat, size_t size);
    void Serialize (std::vector<char>& return_data);
  protected:
    void Deserialize (const char* dat, size_t size);
  private:
    std::string message;
  };

  class BlankMes : public Message {
  public:
    BlankMes (MessageType type, char* dat, size_t size);
    void Serialize (std::vector<char>& return_data);
  protected:
    void Deserialize (const char* dat, size_t size) {}
  };

  class MessageFactory {
  public:
    Message* CreateMessage (MessageType type, char* dat, size_t size);

  protected:
    void Deserialize (const char* dat, size_t size);
  };
} //namespace msg
}//namespace bitmile
#endif
