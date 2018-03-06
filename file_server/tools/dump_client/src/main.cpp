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
    mes = mes_factory.CreateMessage(bitmile::msg::KEYWORD_QUERY, send_buff, size);

    std::vector<char> req_data;
    mes->Serialize(req_data);

    std::cout << "mes type:  " << mes->Type() << " size: " << req_data.size() << std::endl;
    delete mes;
    zmq::message_t request (req_data.size());

    memcpy (request.data(), req_data.data(), req_data.size());

    socket.send(request);

    sleep(10);

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
  }
  return 0;
}
