cmake_minimum_required(VERSION 3.16)

project(Tests VERSION 1.0 LANGUAGES CXX)
enable_testing()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(CTest)

find_package(Threads REQUIRED)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(GTest REQUIRED)

include_directories(${GTEST_INCLUDE_DIRS})
link_libraries(Threads::Threads UtilLibrary ${GTEST_LIBRARIES} ${GTEST_MAIN_LIBRARIES})

add_executable(Commandline Commandline.cpp)
add_executable(Graph Graph.cpp)