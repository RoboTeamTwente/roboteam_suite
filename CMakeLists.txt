cmake_minimum_required(VERSION 3.10)
project(roboteam_proto)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -g -O0")
SET(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake_modules")

# Find programs and assign the boolean to the variables, find_program(variable binary)
find_program(CCACHE ccache)
find_program(GCC g++)
find_program(CLANG clang++)

# Some ccache logic
if (CLANG)
    set(CXX_COMPILER clang++)
elseif (GCC)
    set(CXX_COMPILER g++)
endif()

if (CCACHE)
    set(CMAKE_CXX_COMPILER "/lib/ccache/bin/${CXX_COMPILER}")
else()
    set(CMAKE_CXX_COMPILER "/bin/${CXX_COMPILER}")
endif()
# end of ccache logic

# find the protobuf compiler and libraries
find_package(Protobuf 3.9.1 REQUIRED)
find_package(zeroMQ REQUIRED)
find_package(zmqpp 4.1.2 REQUIRED)

# for zmq to work on mac
set(ENABLE_DRAFTS OFF)

# check if protobuf was found
if(PROTOBUF_FOUND)
    message ("Protobuf 3.9.1 found")
else()
    message (FATAL_ERROR "Cannot find Protobuf")
endif()

# Generate the .h and .cxx files
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS
        proto/Vector2f.proto
        proto/Referee.proto
        proto/RobotCommand.proto
        proto/RobotFeedback.proto
        proto/World.proto
        proto/StringEntry.proto
        proto/Setting.proto

        proto/FieldCircularArc.proto
        proto/FieldLineSegment.proto
        proto/WorldBall.proto
        proto/WorldRobot.proto

        #field
        proto/GeometryData.proto
        proto/GeometryCameraCalibration.proto
        proto/GeometryFieldSize.proto

        #demo
        proto/DemoRobot.proto

        #grsim
        proto/grSim_Commands.proto
        proto/grSim_Packet.proto
        proto/grSim_Replacement.proto

        #mimir
        proto/mimir_robotcommand.proto

        # ssl input packets
        proto/messages_robocup_ssl_detection.proto
        proto/messages_robocup_ssl_game_event.proto
        proto/messages_robocup_ssl_geometry.proto
        proto/messages_robocup_ssl_geometry_legacy.proto
        proto/messages_robocup_ssl_referee.proto
        proto/messages_robocup_ssl_wrapper.proto
        proto/messages_robocup_ssl_wrapper_legacy.proto
        )

# Add an executable
add_library(roboteam_proto SHARED
        ${PROTO_SRCS}
        ${PROTO_HDRS}
        src/Channel.cpp src/Channels.cpp include/roboteam_proto/Channels.h)

# link the exe against the libraries
target_link_libraries(roboteam_proto
        PUBLIC ${PROTOBUF_LIBRARIES}
        # since Publishers/subscribers are templates these 2 libraries need to be PUBLIC
        PUBLIC ${ZeroMQ_LIBRARIES}
        PUBLIC ${ZMQPP_LIBRARIES}
        )

target_include_directories(roboteam_proto
        PUBLIC ${PROTOBUF_INCLUDE_DIRS}
        PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/..
        INTERFACE include
        PRIVATE include/roboteam_proto
        PRIVATE src )


#############
## Testing ##
#############
add_executable(proto_tests
        test/main.cpp
        test/PubSubTest.cpp
        )


if(APPLE)
    target_link_libraries(proto_tests
            PUBLIC roboteam_proto
            PUBLIC roboteam_utils
            /usr/local/lib/libgtest.a
            /usr/local/lib/libgtest_main.a)
endif()

if(UNIX AND NOT APPLE)
    target_link_libraries(proto_tests
            PUBLIC roboteam_proto
            PUBLIC roboteam_utils
            PRIVATE gcov
            PRIVATE gtest
            )
endif()

target_include_directories(proto_tests
        INTERFACE include
        PRIVATE include/roboteam_proto
        PRIVATE src )
