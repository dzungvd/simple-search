#include "zmq_listener.h"

namespace bitmile {
  ZmqListener::ZmqListener() {
    //default contructor

    //init context
    context_ = new zmq::context_t(1);

    //init socket
    clients_socket_ = new zmq::socket_t (*context_, ZMQ_ROUTER);
    workers_socket_ = new zmq::socket_t (*context_, ZMQ_DEALER);

    //TODO: change num workers here
    //init number of workers
    num_workers_ = 5;


    msg_handler_ = new MessageHandler();

    //start network connection
    Start();
  }

  void ZmqListener::Start() {
    pthread_create(&zmq_thread_, NULL, StartInitConnection, this);
  }

  void *ZmqListener::StartWorkerRoutine(void *arg) {
    //TODO: handle pointer casting properly, should not cast directly

    assert (arg != NULL);

    return ((ZmqListener*) arg)->WorkerRoutine(((ZmqListener*) arg)->context_);
  }

  void *ZmqListener::WorkerRoutine(void *arg) {
    assert (arg != NULL);

    //get context object from argument
    zmq::context_t *context = (zmq::context_t *) arg;

    zmq::socket_t socket (*context, ZMQ_REP);
    socket.connect ("inproc://workers");

    while (1) {
      //receive raw message from zmq socket
      zmq::message_t request;
      socket.recv(&request);

      msg::Message* mes;

      if (request.size() >= sizeof (msg::MessageType)) {
        //get message type first
        msg::MessageType type;
        memcpy (&type, (char*)request.data(), sizeof (msg::MessageType));

        //parse raw data into specific message format
        mes = mes_factory_.CreateMessage(type, (char*)request.data() + sizeof (msg::MessageType), request.size() - sizeof (msg::MessageType));

      }else {
        //error - can't parse message type
        mes = mes_factory_.CreateMessage(msg::MessageType::ERROR, NULL, 0);
      }

      //handle input message
      msg::Message* ret_mes = msg_handler_->Handle (mes);

      //make zmq message and send it back to client
      std::vector<char> ret_data;
      ret_mes->Serialize(ret_data);

      zmq::message_t reply (ret_data.size());
      memcpy ((void*) reply.data(), ret_data.data(), ret_data.size());

      //free up memory from intermediate message
      delete mes;
      delete ret_mes;

      socket.send(reply);
    }

    return NULL;
  }

  void *ZmqListener::StartInitConnection(void *arg) {
    assert (arg != NULL);

    return ((ZmqListener*) arg)->InitConnection(NULL);
  }

  void *ZmqListener::InitConnection(void *arg) {

    //TODO: change port to dynamic
    //start accepting connection from clients
    clients_socket_->bind ("tcp://*:5555");

    //init connection for worker threads
    workers_socket_->bind ("inproc://workers");

    //start workers threads
    for (int num_threads = 0; num_threads < num_workers_; num_threads++) {
      pthread_t worker;
      //create threads with worker function here
      pthread_create (&worker, NULL, StartWorkerRoutine, this);
    }

    //link current threads to workers threads
    //add static_cast <void*> here to make zmq work with c++11 or else it will fail to compile
    zmq::proxy (static_cast<void*> (*clients_socket_), static_cast<void*>(*workers_socket_), NULL);

    return NULL;
  }
}//namespace bitmile
