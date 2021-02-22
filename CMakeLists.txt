cmake_minimum_required(VERSION 3.16)
project(tcpsend)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/run)
set(SRC src/main.cpp)
set(BOOST_LIBS Boost::program_options)
set(PCAP_LIBS Pcap++;Packet++;Common++;pcap;pthread)

find_package(Boost REQUIRED program_options)

add_executable(${CMAKE_PROJECT_NAME} ${SRC})
target_link_libraries(${CMAKE_PROJECT_NAME} ${PCAP_LIBS} ${BOOST_LIBS})
