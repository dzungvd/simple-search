#ifndef _ZMQ_LISTENER_H
#define _ZMQ_LISTENER_H

#include <zmq.hpp>
#include <pthread.h>
#include <assert.h>

namespace bitmile {

  /*!
   * ZMQListener class function:
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

  class ZMQListener {
  public:
    ZMQListener();

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

    zmq::context_t *context_; /*!< global context for zmq - allow only one context for each class*/
    zmq::socket_t *clients_socket_; /*!< socket for clients */
    zmq::socket_t *workers_socket_; /*!< socket for workers */

    int num_workers_; /*!< number of working workers */
  };
}//namespace bitmile
#endif
