cmake_minimum_required(VERSION 3.10)
project(roboteam_proto)

SET(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake_modules")

# find the protobuf compiler and libraries
find_package(Protobuf 3.9.1 REQUIRED)
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
        src/Subscriber.cpp
        src/Publisher.cpp)

target_include_directories(roboteam_proto
        PUBLIC ${PROTOBUF_INCLUDE_DIRS}
        PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
        )

# link the exe against the libraries
target_link_libraries(roboteam_proto
        PUBLIC ${PROTOBUF_LIBRARIES}
        PRIVATE ${ZeroMQ_LIBRARIES}
        PRIVATE ${ZMQPP_LIBRARIES}
        )

target_include_directories(roboteam_proto PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        PRIVATE src )
