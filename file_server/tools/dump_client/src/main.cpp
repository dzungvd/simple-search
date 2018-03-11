/*!
 * Dump client is used for testing server connection
 */

#include <zmq.hpp>
#include "message.h"
#include <vector>
#include <string>
#include <iostream>
#include <unistd.h>

int main () {
  zmq::context_t context(1);
  zmq::socket_t socket (context, ZMQ_REQ);
  socket.connect("tcp://localhost:5555");

  bitmile::msg::MessageFactory mes_factory;
  std::vector<std::string> keywords = {"hung", "thinh"};
  int size = 0;
  for (int i = 0; i < keywords.size(); i++) {
    size += keywords[i].length() + 1;
  }
  char* send_buff = new char [size];
  int offset = 0;
  for (int i = 0; i < keywords.size(); i++) {
    memcpy (send_buff + offset, keywords[i].c_str(), keywords[i].length() + 1);
    offset += keywords[i].length() + 1;
  }
  for (int i = 0; i < 100; i++) {

    bitmile::msg::Message* mes;
    if (i % 2 == 0) {
      mes = mes_factory.CreateMessage(bitmile::msg::KEYWORD_QUERY, send_buff, size);

      std::vector<char> req_data;
      mes->Serialize(req_data);

      std::cout << "mes type:  " << mes->Type() << " size: " << req_data.size() << std::endl;
      delete mes;
      zmq::message_t request (req_data.size());

      memcpy (request.data(), req_data.data(), req_data.size());

      socket.send(request);

      zmq::message_t reply;
      socket.recv(&reply);


      bitmile::msg::MessageType type;
      if (sizeof (bitmile::msg::MessageType) > reply.size()) {
        std::cout << "message size smaller than message type variable" << std::endl;
      }
      memcpy (&type, (char*)reply.data(), sizeof (bitmile::msg::MessageType));

      std::cout << "got reply with type: " << type << " data size: " << reply.size() - sizeof (bitmile::msg::MessageType) << std::endl;

      bitmile::msg::Message* recv = mes_factory.CreateMessage (type, ((char*)reply.data()) + sizeof (bitmile::msg::MessageType), reply.size() - sizeof (bitmile::msg::MessageType));

      std::vector <bitmile::db::Document> docs = ((bitmile::msg::KeywordQueryReplyMes*)recv)->GetDocs ();

      std::cout << "num doc: " << docs.size() << std::endl;
      for (int i = 0; i < docs.size() ;i++) {
        std::cout << "doc: " << i << std::endl;
        std::cout << "owner addr: " << docs[i].GetOwnerAddress() << std::endl;
        std::cout << "owner doc id: " << docs[i].GetOwnerDocId() << std::endl;
        std::cout << "elastic doc id: " << docs[i].GetElasticDocId() << std::endl;
      }
    }else {
      //test json:
      nlohmann::json test;
      std::vector<std::string> words = {"thinh", "hung"};
      test["keywords"] = words;

      std::cout << "test: " << test.dump(4) << std::endl;

      for (auto& element : test["keywords"]) {
        std::cout << element.dump() << std::endl;
      }

      //upload doc data
      nlohmann::json json_request = nlohmann::json::parse("{\"owner_address\" : \"ether address\", \"doc_id\": \"1\", \"elastic_doc_id\": \"1\"}");
      std::string raw_str ("encrypted data here");
      unsigned b64_len = sodium_base64_ENCODED_LEN(raw_str.length() + 1, sodium_base64_VARIANT_ORIGINAL);
      char* b64_str = new char[b64_len];

      sodium_bin2base64 (b64_str, b64_len,
                         reinterpret_cast<const unsigned char*> (raw_str.c_str()), raw_str.length() + 1,
                         sodium_base64_VARIANT_ORIGINAL);

      json_request["data"] = b64_str;
      json_request["data_size"] = raw_str.length()+ 1;

      std::vector<std::string> keys = { "a" , "b"};
      json_request["keywords"] = keys;
      delete []b64_str;
      std::string json_str = json_request.dump();
      std::cout << "json: " << json_str << std::endl;

      mes = mes_factory.CreateMessage(bitmile::msg::MessageType::UPLOAD_DOC, json_str.c_str(), json_str.length() + 1);

      std::vector<char> req_data;
      mes->Serialize(req_data);
      std::cout << "mes: " << ((bitmile::msg::UploadDocMes* ) mes)->GetDoc().ToJson() << std::endl;

      std::cout << "mes type:  " << mes->Type() << " size: " << req_data.size() << std::endl;
      delete mes;
      zmq::message_t request (req_data.size());

      memcpy (request.data(), req_data.data(), req_data.size());

      socket.send(request);

            zmq::message_t reply;
      socket.recv(&reply);

      bitmile::msg::MessageType type;
      if (sizeof (bitmile::msg::MessageType) > reply.size()) {
        std::cout << "message size smaller than message type variable" << std::endl;
      }
      memcpy (&type, (char*)reply.data(), sizeof (bitmile::msg::MessageType));

      std::cout << "got reply with type: " << type << " data size: " << reply.size() - sizeof (bitmile::msg::MessageType) << std::endl;

      bitmile::msg::Message* recv = mes_factory.CreateMessage (type, ((char*)reply.data()) + sizeof (bitmile::msg::MessageType), reply.size() - sizeof (bitmile::msg::MessageType));


    }
  }
  return 0;
}
