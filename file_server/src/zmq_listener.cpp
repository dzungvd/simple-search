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

      //parse and handle message here
      msg::Message mes;
      mes.Deserialize((char*)request.data(), request.size());

      msg::Message ret_mes = msg_handler_->Handle (mes);

      //make zmq message and send it back to client
      std::vector<char> ret_data = ret_mes.Serialize();
      zmq::message_t reply (ret_data.size());

      memcpy ((void*) reply.data(), ret_data.data(), ret_data.size());

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
    zmq::proxy (*clients_socket_, *workers_socket_, NULL);

    return NULL;
  }
}//namespace bitmile
