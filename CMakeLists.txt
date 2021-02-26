cmake_minimum_required(VERSION 3.16)
project(tcpsend)

set(SRC src/main.cpp)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_executable(${CMAKE_PROJECT_NAME} ${SRC})
target_link_libraries(${CMAKE_PROJECT_NAME} ${CONAN_LIBS})
