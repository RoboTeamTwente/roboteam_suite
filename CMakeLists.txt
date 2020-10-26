set(PROTO_BASE_PATH proto)

find_package(Protobuf 3.9.1 REQUIRED)

# Generate the .h and .cxx files
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS
        ${PROTO_BASE_PATH}/Vector2f.proto
        ${PROTO_BASE_PATH}/Referee.proto
        ${PROTO_BASE_PATH}/RobotCommand.proto
        ${PROTO_BASE_PATH}/RobotFeedback.proto
        ${PROTO_BASE_PATH}/World.proto
        ${PROTO_BASE_PATH}/StringEntry.proto
        ${PROTO_BASE_PATH}/Setting.proto

        ${PROTO_BASE_PATH}/FieldCircularArc.proto
        ${PROTO_BASE_PATH}/FieldLineSegment.proto
        ${PROTO_BASE_PATH}/WorldBall.proto
        ${PROTO_BASE_PATH}/WorldRobot.proto

        #field
        ${PROTO_BASE_PATH}/GeometryData.proto
        ${PROTO_BASE_PATH}/GeometryCameraCalibration.proto
        ${PROTO_BASE_PATH}/GeometryFieldSize.proto

        ${PROTO_BASE_PATH}/State.proto
        ${PROTO_BASE_PATH}/RobotData.proto
        ${PROTO_BASE_PATH}/RobotParameters.proto
        #demo
        ${PROTO_BASE_PATH}/DemoRobot.proto

        #grsim
        ${PROTO_BASE_PATH}/grSim_Commands.proto
        ${PROTO_BASE_PATH}/grSim_Packet.proto
        ${PROTO_BASE_PATH}/grSim_Replacement.proto

        #mimir
        ${PROTO_BASE_PATH}/mimir_robotcommand.proto

        # ssl input packets
        ${PROTO_BASE_PATH}/messages_robocup_ssl_detection.proto
        ${PROTO_BASE_PATH}/messages_robocup_ssl_game_controller_common.proto
        ${PROTO_BASE_PATH}/messages_robocup_ssl_game_event.proto
        ${PROTO_BASE_PATH}/messages_robocup_ssl_geometry.proto
        ${PROTO_BASE_PATH}/messages_robocup_ssl_geometry_legacy.proto
        ${PROTO_BASE_PATH}/messages_robocup_ssl_referee.proto
        ${PROTO_BASE_PATH}/messages_robocup_ssl_wrapper.proto
        ${PROTO_BASE_PATH}/messages_robocup_ssl_wrapper_legacy.proto
        )

add_library(roboteam_proto STATIC
        ${PROTO_SRCS}
        ${PROTO_HDRS}
        )

target_link_libraries(roboteam_proto
        PUBLIC ${PROTOBUF_LIBRARIES}
        )

target_include_directories(roboteam_proto
        # Proto headers
        PUBLIC ${PROTOBUF_INCLUDE_DIRS}
        # This points to cmake-build-debug/proto/include/proto
        PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/..
        )
