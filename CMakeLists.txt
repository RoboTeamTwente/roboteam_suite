cmake_minimum_required(VERSION 3.0.0)
project(roboteam_central_server VERSION 0.1.0)

add_executable(roboteam_central_server main.cpp)

set_target_properties(roboteam_central_server PROPERTIES
        CMAKE_CXX_STANDARD 20
        CMAKE_CXX_STANDARD_REQUIRED 1
        )

target_link_libraries(roboteam_central_server
        PRIVATE stx
        PRIVATE roboteam_proto
        )

target_include_directories(roboteam_central_server
        PRIVATE ./include
        )