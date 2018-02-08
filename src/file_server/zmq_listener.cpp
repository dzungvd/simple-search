#include "zmq_listener.h"

namespace bitmile {
  ZMQListener::ZMQListener() {
    //default contructor

    //init context
    context_ = new zmq::context_t(1);

    //init socket
    clients_socket_ = new zmq::socket_t (*context_, ZMQ_ROUTER);
    workers_socket_ = new zmq::socket_t (*context_, ZMQ_DEALER);

    //TODO: change num workers here
    //init number of workers
    num_workers_ = 5;

    //start network connection
    Start();
  }

  void ZMQListener::Start() {
    pthread_create(&zmq_thread_, NULL, StartInitConnection, this);
  }

  void *ZMQListener::StartWorkerRoutine(void *arg) {
    //TODO: handle pointer casting properly, should not cast directly

    assert (arg != NULL);

    return ((ZMQListener*) arg)->WorkerRoutine(((ZMQListener*) arg)->context_);
  }

  void *ZMQListener::WorkerRoutine(void *arg) {
    assert (arg != NULL);

    //get context object from argument
    zmq::context_t *context = (zmq::context_t *) arg;

    while (1) {
      //TODO: handle message here
    }

    return NULL;
  }

  void *ZMQListener::StartInitConnection(void *arg) {
    assert (arg != NULL);

    return ((ZMQListener*) arg)->InitConnection(NULL);
  }

  void *ZMQListener::InitConnection(void *arg) {

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
