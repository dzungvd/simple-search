#ifndef _MESSAGE_H
#define _MESSAGE_H

#include <vector>

namespace bitmile{
namespace msg {

  //message exchange between clients and servers

  enum MessageType {
    KEYWORD_QUERY,
  };

  struct Message {
    MessageType type;
    std::vector<char> data;
  };

} //namespace msg
}//namespace bitmile
#endif
