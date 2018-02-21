#include "message_handler.h"

namespace bitmile {

  msg::Message MessageHandler::Handle (msg::Message mes) {
    switch (mes.type) {
    case msg::KEYWORD_QUERY:
      //forward query to database server
      break;
    default:
      break;
    }
  }

}//namespace bitmile
