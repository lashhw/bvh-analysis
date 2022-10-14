cmake_minimum_required(VERSION 3.23)
project(bvh_analysis)

set(CMAKE_CXX_STANDARD 17)

add_executable(visualize visualize/main.cpp)
add_subdirectory(third_party/bvh)
target_link_libraries(visualize PUBLIC bvh)
