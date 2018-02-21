#include "message_handler.h"

namespace bitmile {

  msg::Message MessageHandler::Handle (msg::Message mes) {
    msg::Message reply;
    switch (mes.type) {
    case msg::KEYWORD_QUERY:
      //TODO: forward query to database server
      reply.type = msg::KEYWORD_QUERY_REPLY;
      break;
    default:
      reply.type = msg::BLANK;
      break;
    }
    return reply;
  }

}//namespace bitmile
