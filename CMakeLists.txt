cmake_minimum_required(VERSION 3.0.0)
project(UtilLibrary VERSION 1.0.0 LANGUAGES CXX DESCRIPTION "A header only library made for providing extended functionality for your C++ project.")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Add library
add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_20)

if (${EXAMPLES})
    add_subdirectory("tests")
endif()