#ifndef _MESSAGE_HANDLER_H
#define _MESSAGE_HANDLER_H

#include "message.h"
namespace bitmile{
  class MessageHandler {
  public:
    MessageHandler() {}
    /*! detect types and handle message of each type*/
    msg::Message Handle (msg::Message mes);

  private:
    //database interface

  };
}//namespace bitmile
#endif
