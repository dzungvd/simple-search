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
    msg::KeywordQueryMes* keyword_query = static_cast <msg::KeywordQueryMes*> (query);

    std::vector<db::Document> results;

    //send query to database
    db_.QueryDocWithKeywords (keyword_query->GetKeywords(), results);

    /*
    for (int i = 0; i < results.size(); i++) {
      std::cout << "result " << i << std::endl
                << "address " << results[i].GetOwnerAddress() << std::endl
                << "docid: " << results[i].GetOwnerDocId() << std::endl
                << "elastic id: " <<results[i].GetElasticDocId() << std::endl;
                }*/
    //create a reply message
    msg::Message* reply = mes_factory_.CreateMessage (msg::MessageType::KEYWORD_QUERY_REPLY, NULL, 0);

    (static_cast <msg::KeywordQueryReplyMes*> (reply))->SetDocuments (results);

    return reply;
  }
}//namespace bitmile
