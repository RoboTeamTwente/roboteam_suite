cmake_minimum_required(VERSION 3.0.0)
project(roboteam_central_server VERSION 0.1.0)

add_executable(roboteam_central_server 
        main.cpp
        src/server.cpp
        )

set_target_properties(roboteam_central_server PROPERTIES
        CMAKE_CXX_STANDARD 20
        CMAKE_CXX_STANDARD_REQUIRED 1
        )

target_link_libraries(roboteam_central_server
        PUBLIC stx
        PUBLIC networking
        PUBLIC roboteam_proto
        PUBLIC roboteam_utils
        )

target_include_directories(roboteam_central_server
        PUBLIC ./include
        )

add_executable(roboteam_central_server_tests
        src/server.cpp
        test/pair_test.cpp
        test/main.cpp
        )

target_include_directories(roboteam_central_server_tests
        PUBLIC ./include
        )

target_link_libraries(roboteam_central_server_tests        
        PUBLIC stx
        PUBLIC gtest
        PUBLIC networking
        PUBLIC roboteam_proto
        PUBLIC roboteam_utils
        )