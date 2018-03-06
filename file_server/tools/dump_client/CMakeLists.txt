set (PROJ_NAME dump_client)
set (PROJ_INCLUDES "inc;../../inc")
set (PROJ_SOURCE "src/main.cpp" "../../src/message.cpp" "../../src/db_interface.cpp")


#add elastic cpp to our project
set(ELASTIC_CPP_FOLDER "../../vendors/cpp-elasticsearch/src")
set(ELASTIC_CPP_SOURCE "${ELASTIC_CPP_FOLDER}/elasticsearch/elasticsearch.cpp" "${ELASTIC_CPP_FOLDER}/json/json.cpp" "${ELASTIC_CPP_FOLDER}/http/http.cpp")

set(ELASTIC_CPP_INC "../../vendors/cpp-elasticsearch/src")

list(APPEND PROJ_SOURCE ${ELASTIC_CPP_SOURCE})


INCLUDE_DIRECTORIES("${PROJ_INCLUDES}" "${ELASTIC_CPP_INC}")

add_executable(${PROJ_NAME} ${PROJ_SOURCE})

## add the include directory to our compile directives
target_include_directories(${PROJ_NAME} PUBLIC ${ZeroMQ_INCLUDE_DIR})
## at the 0mq library to our link directive
target_link_libraries(${PROJ_NAME} PUBLIC ${ZeroMQ_LIBRARY})
