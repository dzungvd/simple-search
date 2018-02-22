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
  for (int i = 0; i < 100; i++) {

    bitmile::msg::Message mes;
    mes.type = bitmile::msg::KEYWORD_QUERY;

    std::vector<char> req_data = mes.Serialize();
    zmq::message_t request (req_data.size());

    memcpy (request.data(), req_data.data(), req_data.size());

    socket.send(request);

    sleep(10);

    zmq::message_t reply;
    socket.recv(&reply);

    bitmile::msg::Message rep_mes;
    rep_mes.Deserialize ((char*) reply.data(), reply.size());

    std::cout << "got reply with type: " << rep_mes.type << " data size: " << rep_mes.data.size() << std::endl;
  }
  return 0;
}
