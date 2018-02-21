#include "message_hander.h"

namespace bitmile {

  msg::Message MessageHandler::Hanle (msg::Message mes) {
    switch (mes.type) {
    case msg::KEYWORD_QUERY:
      //forward query to database server
      break;
    default:
      break;
    }
  }

}//namespace bitmile
