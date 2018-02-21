#ifndef _MESSAGE_H
#define _MESSAGE_H

#include <vector>
#include <cstdlib>
#include <assert.h>
#include <cstring>

namespace bitmile{
namespace msg {

  //message exchange between clients and servers

  enum MessageType {
    KEYWORD_QUERY,
    KEYWORD_QUERY_REPLY,
    BLANK,
  };

  struct Message {
    MessageType type;
    std::vector<char> data;

    void Deserialize(char* dat, size_t size) {
      assert (size >= sizeof(MessageType));
      memcpy (&type, dat, sizeof(MessageType));
      dat += sizeof (MessageType);
      size -= sizeof (MessageType);
      data.resize(size);
      if (size > 0) {
        memcpy (data.data(), dat, size);
      }
    }

    std::vector<char> Serialize(){
      std::vector<char> bin;
      int size = sizeof (MessageType) + data.size();
      bin.resize(size);

      int offset = 0;

      memcpy (&bin[offset], &type, sizeof (MessageType));

      size -= sizeof(MessageType);
      offset += sizeof(MessageType);
      if (size > 0){
        memcpy (&bin[offset], data.data(), size);
      }
      return bin;
    }
  };

} //namespace msg
}//namespace bitmile
#endif
