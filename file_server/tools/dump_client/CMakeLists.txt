set (PROJ_NAME dump_client)
set (PROJ_INCLUDES "inc;../../inc")
set (PROJ_SOURCE "src/main.cpp" "../../src/message.cpp")

INCLUDE_DIRECTORIES(${PROJ_INCLUDES})

add_executable(${PROJ_NAME} ${PROJ_SOURCE})

## add the include directory to our compile directives
target_include_directories(${PROJ_NAME} PUBLIC ${ZeroMQ_INCLUDE_DIR})
## at the 0mq library to our link directive
target_link_libraries(${PROJ_NAME} PUBLIC ${ZeroMQ_LIBRARY})
