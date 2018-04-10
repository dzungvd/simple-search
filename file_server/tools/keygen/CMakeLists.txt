set (PROJ_NAME keygen)
set (PROJ_INCLUDES "../../inc/" "${CMAKE_SOURCE_DIR}")
set (PROJ_SOURCE "src/main.cpp" "../../src/message_crypto.cpp")

include_directories ("${PROJ_INCLUDES}")

add_executable (${PROJ_NAME} ${PROJ_SOURCE})

target_include_directories (${PROJ_NAME} PUBLIC ${sodium_INCLUDE_DIR})
target_link_libraries (${PROJ_NAME} PUBLIC ${sodium_LIBRARY_RELEASE})
