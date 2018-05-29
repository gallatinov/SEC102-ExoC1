# CMakeLists files in this project can
# refer to the root source directory of the project as ${HELLO_SOURCE_DIR} and
# to the root binary directory of the project as ${HELLO_BINARY_DIR}.
cmake_minimum_required (VERSION 2.8.11)
project (SEC102)

SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/../bin) 

if (MSVC)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
endif()

add_subdirectory(listdir)
add_subdirectory(listproc)
add_subdirectory(wininet)
add_subdirectory(sock)
add_subdirectory(rot13)