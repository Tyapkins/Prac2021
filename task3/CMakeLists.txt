cmake_minimum_required(VERSION 3.17)
project(task3)

SET(CMAKE_CXX_STANDARD 20)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)

#enable_testing()
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -lgtest  -pthread" )
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lgtest  -pthread")

find_package(Threads REQUIRED)
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})
include_directories(.)

add_executable(task3
        types.h
        factory.h
        graphs/graph.h
        graphs/simple.h
        graphs/complete.h
        graphs/bipartite.h
        graphs/weighted.h
        graphs/graph.cpp
        graphs.h
        funcs.h
        distance.h
        graphs/simple.cpp
        graphs/complete.cpp
        graphs/bipartite.cpp
        graphs/weighted.cpp
        factory.cpp
        distance.cpp
        main.cpp
        tests/test_graph.cpp
        tests/test_creation.cpp
        tests/test_distance.cpp)

target_link_libraries(task3 ${GTEST_LIBRARIES})