#include "message_handler.h"

namespace bitmile {

  msg::Message* MessageHandler::Handle (msg::Message* mes) {
    msg::Message* reply;
    switch (mes->Type()) {
    case msg::KEYWORD_QUERY:
      //TODO: forward query to database server
      reply = HandleKeywordQuery (mes);
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

  msg::Message* MessageHandler::HandleKeywordQuery (msg::Message* query) {

    //get all user encrypt password
    //encrypt all keywords and send query to database server

  }
}//namespace bitmile
