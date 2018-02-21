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

    while (1) {
      //TODO: parse and handle message here
      msg::Message mes;
      msg::Message ret_mes = msg_handler_->Handle (mes);

      //TODO: change ret_mes to zmq format
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
    clients_socket_->bind ("tcp://*5555");

    //init connection for worker threads
    workers_socket_->bind ("inproc://workers");

    //start workers threads
    for (int num_threads = 0; num_threads < num_workers_; num_threads++) {
      pthread_t worker;
      //TODO: create threads with worker function herer
      pthread_create (&worker, NULL, StartWorkerRoutine, this);
    }

    //link current threads to workers threads
    zmq::proxy (clients_socket_, workers_socket_, NULL);

    return NULL;
  }
}//namespace bitmile
