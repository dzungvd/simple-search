#file server cmake file

set(PROJ_NAME file_server)
set(PROJ_INCLUDES "inc")
set(PROJ_SOURCE "src/main.cpp" "src/zmq_listener.cpp" "src/message.cpp" "src/message_handler.cpp")

INCLUDE_DIRECTORIES(${PROJ_INCLUDES})

add_executable(${PROJ_NAME} ${PROJ_SOURCE})

#link pthreads
find_package(Threads REQUIRED)
if(THREADS_HAVE_PTHREAD_ARG)
  target_compile_options(${PROJ_NAME} PUBLIC "-pthread")
endif()
if(CMAKE_THREAD_LIBS_INIT)
  target_link_libraries(${PROJ_NAME} PUBLIC ${CMAKE_THREAD_LIBS_INIT})
endif()

#link zero zmq through pkg-config hack
## load in pkg-config support
find_package(PkgConfig)
## use pkg-config to get hints for 0mq locations
pkg_check_modules(PC_ZeroMQ QUIET zmq)

## use the hint from above to find where 'zmq.hpp' is located
find_path(ZeroMQ_INCLUDE_DIR
  NAMES zmq.hpp
  PATHS ${PC_ZeroMQ_INCLUDE_DIRS}
  )

## use the hint from about to find the location of libzmq
find_library(ZeroMQ_LIBRARY
  NAMES zmq
  PATHS ${PC_ZeroMQ_LIBRARY_DIRS}
  )

## add the include directory to our compile directives
target_include_directories(${PROJ_NAME} PUBLIC ${ZeroMQ_INCLUDE_DIR})
## at the 0mq library to our link directive
target_link_libraries(${PROJ_NAME} PUBLIC ${ZeroMQ_LIBRARY})

add_subdirectory(tools)
