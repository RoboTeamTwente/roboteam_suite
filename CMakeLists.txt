cmake_minimum_required(VERSION 3.16)
project(roboteam_central_server VERSION 0.1.0)

add_executable(roboteam_central_server 
        main.cpp
        src/server.cpp
        )

target_link_libraries(roboteam_central_server
        PRIVATE stx
        PRIVATE roboteam_utils
        PRIVATE ixwebsocket
        PRIVATE roboteam_proto
        PRIVATE roboteam_utils
        )

target_include_directories(roboteam_central_server
        PRIVATE include/
        )

add_executable(roboteam_central_server_tests
        test/main.cpp
        src/server.cpp
        test/pair_test.cpp
        )

target_include_directories(roboteam_central_server_tests
        PUBLIC ./include
        )

target_link_libraries(roboteam_central_server_tests        
        PUBLIC stx
        PUBLIC gtest
        PUBLIC roboteam_utils
        PUBLIC ixwebsocket
        PUBLIC roboteam_proto
        PUBLIC roboteam_utils
        )