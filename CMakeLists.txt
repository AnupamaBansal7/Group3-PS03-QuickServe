cmake_minimum_required(VERSION 3.10)
project(QuickServe CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include tests)

file(GLOB_RECURSE SRC_FILES
    "src/*.cpp"
    "tests/TestSuite.cpp"
)

add_executable(quickserve ${SRC_FILES})
