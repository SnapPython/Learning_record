cmake_minimum_required(VERSION 2.6)

project(test)

set(CMAKE_CXX_STANDARD 11)

find_package(OpenCV 4.8 REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(test test.cpp)

target_link_libraries(test ${OpenCV_LIBS})


