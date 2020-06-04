cmake_minimum_required(VERSION 3.10)
project(rtt)

set(CMAKE_CXX_FLAGS_DEBUG "-std=c++17 ${CMAKE_CXX_FLAGS} -Wall -Wextra -fPIE -O1")
set(CMAKE_CXX_FLAGS_RELEASE "-std=c++17 ${CMAKE_CXX_FLAGS} -fPIE -Wall -Wextra -Werror -O1")

include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)

ExternalProject_Add(sdl2
        SOURCE_DIR ${CMAKE_SOURCE_DIR}/thirdparty/sdl2
        )

ExternalProject_Add(eigen3
        SOURCE_DIR ${CMAKE_SOURCE_DIR}/thirdparty/eigen3
        )

ExternalProject_Add(armadillo
        SOURCE_DIR ${CMAKE_SOURCE_DIR}/thirdparty/armadillo
        )

ExternalProject_Add(nfparam
        SOURCE_DIR ${CMAKE_SOURCE_DIR}/thirdparty/nfparam
        )

ExternalProject_Add(stx
        SOURCE_DIR ${CMAKE_SOURCE_DIR}/thirdparty/stx
        )

add_subdirectory(thirdparty/gtest)

add_subdirectory(roboteam_proto)
add_subdirectory(roboteam_utils)
add_subdirectory(roboteam_ai)
add_subdirectory(roboteam_robothub)
add_subdirectory(roboteam_world)
