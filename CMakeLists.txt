cmake_minimum_required(VERSION 3.16)
project(tcpsend)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

set(SRC src/main.cpp)

if (UNIX)
    find_package(Boost REQUIRED program_options)
    set(BOOST_LIBS Boost::program_options)
    set(PCAP_LIBS Pcap++;Packet++;Common++;pcap;pthread)
    set(LIBRARIES ${PCAP_LIBS} ${BOOST_LIBS})
else()
    set(LIBRARIES ${CONAN_LIBS})
endif()

add_executable(${CMAKE_PROJECT_NAME} ${SRC})
target_link_libraries(${CMAKE_PROJECT_NAME} ${LIBRARIES})
