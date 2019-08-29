cmake_minimum_required(VERSION 3.4)
project(roboteam_proto)

#SET(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS} -g -O0 -Wall --coverage")
set(CMAKE_CXX_FLAGS_DEBUG "-std=c++17 ${CMAKE_CXX_FLAGS} -O1")
set(CMAKE_CXX_FLAGS_RELEASE "-std=c++17 ${CMAKE_CXX_FLAGS} -Werror -O1")

SET(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake_modules")


# find the protobuf compiler and libraries
find_package(Protobuf REQUIRED)
find_package(zeroMQ REQUIRED)
find_package(zmqpp REQUIRED)

# for zmq to work on mac
set(ENABLE_DRAFTS OFF)

# check if protobuf was found
if(PROTOBUF_FOUND)
    message ("protobuf found")
else()
    message (FATAL_ERROR "Cannot find Protobuf")
endif()

# Generate the .h and .cxx files
PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS
        proto/Vector2f.proto
        proto/Referee.proto
        proto/RobotCommand.proto
        proto/RobotFeedback.proto
        proto/World.proto
        proto/StringEntry.proto

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

        # ssl input packets
        proto/messages_robocup_ssl_detection.proto
        proto/messages_robocup_ssl_game_event.proto
        proto/messages_robocup_ssl_geometry.proto
        proto/messages_robocup_ssl_geometry_legacy.proto
        proto/messages_robocup_ssl_referee.proto
        proto/messages_robocup_ssl_wrapper.proto
        proto/messages_robocup_ssl_wrapper_legacy.proto
        )

# Print path to generated files
message ("PROTO_SRCS = ${PROTO_SRCS}")
message ("PROTO_HDRS = ${PROTO_HDRS}")

# Add an executable
add_library(roboteam_proto SHARED
        ${PROTO_SRCS}
        ${PROTO_HDRS}
        src/Subscriber.cpp
        src/Publisher.cpp)

target_include_directories(roboteam_proto
        PUBLIC
        ${PROTOBUF_INCLUDE_DIRS}
        ${CMAKE_CURRENT_BINARY_DIR}
        )

# link the exe against the libraries
target_link_libraries(roboteam_proto
        PUBLIC
        ${PROTOBUF_LIBRARIES}
        ${ZeroMQ_LIBRARIES}
        ${ZMQPP_LIBRARIES}
        )

target_include_directories(roboteam_proto PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        PRIVATE src )
