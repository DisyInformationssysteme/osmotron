cmake_minimum_required(VERSION 3.5)
project(osmotron)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
find_package(Boost REQUIRED)
include_directories(SYSTEM ${Boost_INCLUDE_DIRS})
find_package(Osmium REQUIRED COMPONENTS <io xml sparsehash>)
include_directories(SYSTEM ${OSMIUM_INCLUDE_DIRS})

set(SOURCE_FILES main.cpp)
set(PROG osmotron)
add_executable(${PROG} ${SOURCE_FILES})
target_link_libraries(${PROG} ${Boost_LIBRARIES} ${OSMIUM_LIBRARIES})