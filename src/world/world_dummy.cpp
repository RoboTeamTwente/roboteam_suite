#include "world_dummy.h"

#include <iostream>


namespace rtt {

    void WorldDummy::reset() {
        robots_yellow.clear();
        robots_blue.clear();
        ball = Ball();
    }


    void WorldDummy::detection_callback(const roboteam_msgs::DetectionFrame msg) {

        if (msg.camera_id < config.num_cams()) {

            std::vector<roboteam_msgs::DetectionRobot> yellow = msg.robots_yellow;
            std::vector<roboteam_msgs::DetectionRobot> blue = msg.robots_blue;

            std::vector<roboteam_msgs::DetectionBall> balls = msg.balls;

            for (int i = 0; i < yellow.size(); ++i)
            {
                robots_yellow[i].set_id(yellow[i].robot_id);
                robots_yellow[i].move_to(yellow[i].x, yellow[i].y);
                robots_yellow[i].rotate_to(yellow[i].orientation);
            }

            for (int i = 0; i < blue.size(); ++i)
            {
                robots_blue[i].set_id(blue[i].robot_id);
                robots_blue[i].move_to(blue[i].x, blue[i].y);
                robots_blue[i].rotate_to(blue[i].orientation);
            }

            if (balls.size() > 0) {
                ball.set_area(balls[0].area);
                ball.move_to(balls[0].x, balls[0].y, balls[0].z);
            }

        }

    }


    roboteam_msgs::World WorldDummy::as_message() {
        roboteam_msgs::World msg;

        for (std::vector<rtt::Robot>::iterator it = robots_blue.begin(); it != robots_blue.end(); it++) {
            msg.robots_blue.push_back(it->as_message());
        }

        for (std::vector<rtt::Robot>::iterator it = robots_yellow.begin(); it != robots_yellow.end(); it++) {
            msg.robots_yellow.push_back(it->as_message());
        }

        msg.ball = ball.as_message();

        return msg;
    }

}
