#pragma once

#include "ros/ros.h"

#include "std_srvs/Empty.h"

#include "roboteam_msgs/DetectionFrame.h"
#include "roboteam_msgs/World.h"
#include "roboteam_msgs/Tracker.h"

#include "roboteam_world/world/world_dummy.h"
#include "roboteam_world/tracker/opponent_tracker.h"
#include "roboteam_world/tracker/acceleration_tracker.h"
#include "roboteam_world/tracker/speed_tracker.h"


namespace rtt {

    class RosHandler {

    private:
        ros::NodeHandle nh;
        ros::Subscriber vision_sub;
        ros::Publisher world_pub;
        ros::ServiceServer reset_srv;
        ros::ServiceServer tracker_srv;

        WorldBase* world;
        OpponentTracker tracker;

    public:
        RosHandler();
        void init(WorldBase* _world);

        /**
         * Reads the configuration from the parameter server.
         * Updates the configuration of the world and calls a reset.
         */
        void update_config();

        void detection_callback(const roboteam_msgs::DetectionFrame msg);
        bool reset_callback(std_srvs::Empty::Request& req,
                            std_srvs::Empty::Response& res);
                            
        bool tracker_callback(roboteam_msgs::Tracker::Request& req,
                              roboteam_msgs::Tracker::Response& res);
        TrackerResult* track(const std::string& type, const RobotID& id) const;
    };

}
