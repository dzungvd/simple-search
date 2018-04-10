set (PROJ_NAME dump_client)
set (PROJ_INCLUDES "inc;${CMAKE_SOURCE_DIR}")
set (PROJ_SOURCE "src/main.cpp" "${CMAKE_SOURCE_DIR}/message/message.cpp" "${CMAKE_SOURCE_DIR}/database/db_interface.cpp")

list(APPEND PROJ_SOURCE ${ELASTIC_CPP_SOURCE})

INCLUDE_DIRECTORIES("${PROJ_INCLUDES}" "${ELASTIC_CPP_INC}" "${JSON_FOLDER}")

add_executable(${PROJ_NAME} ${PROJ_SOURCE})

## add the include directory to our compile directives
target_include_directories(${PROJ_NAME} PUBLIC ${ZeroMQ_INCLUDE_DIR})
## at the 0mq library to our link directive
target_link_libraries(${PROJ_NAME} PUBLIC ${ZeroMQ_LIBRARY})

target_include_directories(${PROJ_NAME} PUBLIC ${sodium_INCLUDE_DIR})
target_link_libraries(${PROJ_NAME} PUBLIC ${sodium_LIBRARY_RELEASE})
