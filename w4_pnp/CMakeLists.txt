cmake_minimum_required(VERSION 3.12)
project(pnp)

find_package(OpenCV REQUIRED)

add_executable(pnp pnp.cpp)

target_link_libraries(pnp ${OpenCV_LIBS})

