cmake_minimum_required(VERSION 3.12)
project(rec)

find_package(OpenCV REQUIRED)

add_executable(rec ArmorDetector.cpp numeric_rm.cpp opencv_extended.cpp rec.cpp)

target_link_libraries(rec ${OpenCV_LIBS})

