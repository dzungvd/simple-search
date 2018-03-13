#include "message_handler.h"

namespace bitmile {

  msg::Message* MessageHandler::Handle (msg::Message* mes) {
    msg::Message* reply;
    switch (mes->Type()) {
    case msg::KEYWORD_QUERY:
      //TODO: forward query to database server
      reply = HandleKeywordQuery (mes);
      break;
    case msg::UPLOAD_DOC:
      reply = HandleUploadDoc (mes);
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

  msg::Message* MessageHandler::HandleUploadDoc (msg::Message* mes) {
    msg::UploadDocMes* upload_doc_mes = static_cast<msg::UploadDocMes*> (mes);

    const db::Document& doc = upload_doc_mes->GetDoc();

    //document must contain data, owner address, keywords and id

    std::string owner_address = doc.GetOwnerAddress();
    std::string doc_id = doc.GetOwnerDocId();
    const std::vector<char>& data = doc.GetData();

    if (owner_address.length() == 0 ||
        doc_id.length() == 0 ||
        data.size() == 0) {
      //return error code here
    }

    //put owner_address, doc_id, keywords to elastic db
    db::Document elastic_doc;
    elastic_doc.SetOwnerAddress (doc.GetOwnerAddress());
    elastic_doc.SetOwnerDocId (doc.GetOwnerDocId ());
    elastic_doc.SetKeywords (doc.GetKeywords());

    std::string elastic_doc_id = db_.InsertDoc (elastic_doc);
    msg::Message* reply;

    if (elastic_doc_id.length() > 0) {
      //save data to file

      //check and create folder if not exist
      std::string path = boost::filesystem::current_path().string() + std::string("/data");

      boost::filesystem::path dir (path);

      boost::system::error_code ec;

      boost::filesystem::create_directory (dir);

      //get file name from elastic id
      std::string file_path_str = path + std::string("/") + elastic_doc_id;
      boost::filesystem::path file_path (file_path_str);


      std::ofstream fout (file_path_str, std::ios::out | std::ios::binary );
      fout.write (doc.GetData().data(), doc.GetData().size());
      fout.close();

      int code = 200;
      reply = mes_factory_.CreateMessage (msg::MessageType::UPLOAD_DOC_REPLY, (char*)&code, (size_t)(sizeof (int)));
      return reply;
    }


    //error
    reply = mes_factory_.CreateMessage (msg::MessageType::ERROR, NULL, 0);
    return reply;
  }
}//namespace bitmile
