#ifndef _MESSAGE_HANDLER_H
#define _MESSAGE_HANDLER_H

#include "message.h"
#include "db_interface.h"
namespace bitmile{
  class MessageHandler {
  public:
    MessageHandler() {}
    /*! detect types and handle message of each type*/
    msg::Message Handle (msg::Message mes);

  private:
    /*!< database interface */
    DbInterface db_;
  };
}//namespace bitmile
#endif
