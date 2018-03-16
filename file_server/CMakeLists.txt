#file server cmake file

set(PROJ_NAME file_server)
set(PROJ_INCLUDES "inc")
set(PROJ_SOURCE "src/main.cpp" "src/zmq_listener.cpp" "src/message.cpp" "src/message_handler.cpp" "src/db_interface.cpp")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/")
message ("cmake module ${CMAKE_MODULE_PATH}")

#add elastic cpp to our project
set(ELASTIC_CPP_FOLDER "${CMAKE_CURRENT_SOURCE_DIR}/vendors/cpp-elasticsearch/src")
set(ELASTIC_CPP_SOURCE "${ELASTIC_CPP_FOLDER}/elasticsearch/elasticsearch.cpp" "${ELASTIC_CPP_FOLDER}/json/json.cpp" "${ELASTIC_CPP_FOLDER}/http/http.cpp")

set(ELASTIC_CPP_INC "${CMAKE_CURRENT_SOURCE_DIR}/vendors/cpp-elasticsearch/src")

list(APPEND PROJ_SOURCE ${ELASTIC_CPP_SOURCE})

#add json library
set(JSON_FOLDER "${CMAKE_CURRENT_SOURCE_DIR}/vendors/nlohmann_json")

INCLUDE_DIRECTORIES("${PROJ_INCLUDES}" "${ELASTIC_CPP_INC}" "${JSON_FOLDER}")

add_executable(${PROJ_NAME} ${PROJ_SOURCE})


#find required package
find_package(Threads REQUIRED)
find_package(ZeroMQ REQUIRED)
find_package (sodium REQUIRED)
find_package(Boost COMPONENTS system filesystem REQUIRED)
find_package(OpenSSL COMPONENTS SSL Crypto REQUIRED)

#link pthreads
if(THREADS_HAVE_PTHREAD_ARG)
  target_compile_options(${PROJ_NAME} PUBLIC "-pthread")
endif()
if(CMAKE_THREAD_LIBS_INIT)
  target_link_libraries(${PROJ_NAME} PUBLIC ${CMAKE_THREAD_LIBS_INIT})
endif()

## add the include directory to our compile directives
target_include_directories(${PROJ_NAME} PUBLIC ${ZeroMQ_INCLUDE_DIRS})
## at the 0mq library to our link directive
target_link_libraries(${PROJ_NAME} PUBLIC ${ZeroMQ_LIBRARIES})

#sodium library
#use static lib
set(sodium_USE_STATIC_LIBS TRUE)

target_include_directories(${PROJ_NAME} PUBLIC ${sodium_INCLUDE_DIR})
target_link_libraries(${PROJ_NAME} PUBLIC ${sodium_LIBRARY_RELEASE})

#use boost lib

set(Boost_USE_STATIC_LIBS        ON)  # only find static libs
target_link_libraries (${PROJ_NAME} PUBLIC ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY})


#use openssl

set (OPENSSL_USE_STATIC_LIBS TRUE)
target_include_directories(${PROJ_NAME} PUBLIC ${OpenSSL_INCLUDE_DIR})
target_link_libraries(${PROJ_NAME} PUBLIC ${OpenSSL_LIBRARIES})

add_subdirectory(tools)
