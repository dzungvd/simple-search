#ifndef _ZMQ_LISTENER_H
#define _ZMQ_LISTENER_H

#include <zmq.hpp>
#include "zhelpers.hpp"
#include <pthread.h>
#include <assert.h>
#include <queue>
#include "message/message.h"
#include "utils/utils.h"
#include "message_handler.h"
#include "message_crypto.h"

namespace bitmile {

  /*!
   * ZmqListener class function:
   * 1. receive message from outside clients
   * 2. pass messages to multiple workers for processing
   * 3. send result back to clients
   *
   * ZMQListerner class handle
   * - communication between file server and clients
   * - distribute messages command to different worker threads
   *
   * Based on "Multithreading with ZeroMQ" model
   * more info here: http://zguide.zeromq.org/page:all#Multithreading-with-ZeroMQ
   */

  class ZmqListener {
  public:
    ZmqListener();

    ~ZmqListener() {
      delete context_;
      delete clients_socket_;
      delete workers_socket_;
      delete msg_handler_;
    }
  private:
    pthread_t zmq_thread_ ; /*!< thread for clients listener */

    /*! create zmq_thread_ */
    void Start();

    /*!
     * start listening to clients
     * create workers' thread and connection to them
     */
    void *InitConnection (void* arg); /*! create */

    /*! workaround to use non-static function for thread */
    static void *StartInitConnection (void* arg);

    /*! worker logic: receive and handle incoming message */
    void *WorkerRoutine (void* arg);

    static void *StartWorkerRoutine (void * arg);

    static void ClientBalancingMsgBroker (zmq::socket_t& frontend, zmq::socket_t& backend, int worker_num);

    zmq::context_t *context_; /*!< global context for zmq - allow only one context for each class*/
    zmq::socket_t *clients_socket_; /*!< socket for clients */
    zmq::socket_t *workers_socket_; /*!< socket for workers */

    int num_workers_; /*!< number of working workers */

    MessageHandler* msg_handler_; /*!< pointer to message handler class */
    msg::MessageFactory mes_factory_;
  };
}//namespace bitmile
#endif
