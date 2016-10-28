#pragma once

#include "roboteam_msgs/WorldRobot.h"
#include "roboteam_utils/Position.h"

#define INVALID_ROBOT_ID 99999

namespace rtt {

    class Robot {

    private:
        uint id;
        float x;
        float y;
        float angle;

        float x_vel;
        float y_vel;
        float w;

    public:
        Robot();
        Robot(uint id);
        Robot(uint id, float x, float y, float w);

        void set_id(uint id);
        void move_to(float x, float y);
        void rotate_to(float w);
        void set_vel(float x_vel, float y_vel, float w_vel);

        roboteam_utils::Position get_position() const;
        roboteam_utils::Position get_velocity() const;
        uint get_id() const;

       roboteam_msgs::WorldRobot as_message() const;
    };

}
