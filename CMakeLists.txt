cmake_minimum_required(VERSION 3.23)
project(bvh_analysis)

set(CMAKE_CXX_STANDARD 17)

add_executable(visualize visualize/main.cpp)
add_subdirectory(third_party/bvh)
target_link_libraries(visualize PUBLIC bvh)

add_executable(collect_trace collect_trace/main.cpp)
target_link_libraries(collect_trace PUBLIC bvh)

add_executable(obj_to_ply obj_to_ply/main.cpp)

add_executable(gen_coe_file gen_coe_file/main.cpp)
target_link_libraries(gen_coe_file PUBLIC bvh)

add_executable(merge_ply merge_ply/main.cpp)