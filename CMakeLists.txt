cmake_minimum_required(VERSION 3.4)

if(POLICY CMP0077)
    cmake_policy(SET CMP0077 NEW)
endif()

set(COVERAGE OFF CACHE BOOL "Coverage")
set(CMAKE_CXX_STANDARD 17)  
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# set(CMAKE_CXX_COMPILER "/usr/bin/g++")

project(TestRunning LANGUAGES CXX)

find_package(GTest REQUIRED)
find_package(Threads REQUIRED)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/banking)

add_executable(RunTest 
    ${CMAKE_CURRENT_SOURCE_DIR}/test.cpp
)

if(COVERAGE)
    target_compile_options(RunTest PRIVATE --coverage)
    target_link_libraries(RunTest PRIVATE --coverage)
endif()

target_include_directories(RunTest PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/banking
    ${GTEST_INCLUDE_DIRS}
)


target_link_libraries(RunTest PRIVATE 
    banking
    GTest::GTest 
    GTest::Main
    GMock::GMock
    Threads::Threads
)

include(GoogleTest)
gtest_discover_tests(RunTest)
