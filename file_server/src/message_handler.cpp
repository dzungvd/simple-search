#include "message_handler.h"

namespace bitmile {

  msg::Message* MessageHandler::Handle (msg::Message* mes) {
    msg::Message* reply;
    switch (mes->Type()) {
    case msg::KEYWORD_QUERY:
      //TODO: forward query to database server
      reply = mes_factory_.CreateMessage (msg::MessageType::KEYWORD_QUERY_REPLY, NULL, 0);
      break;
    case msg::BLANK:
      reply = mes_factory_.CreateMessage (msg::MessageType::BLANK, NULL, 0);
      break;
    default:
      reply = mes_factory_.CreateMessage (msg::MessageType::ERROR, NULL, 0);
      break;
    }
    return reply;
  }

}//namespace bitmile
