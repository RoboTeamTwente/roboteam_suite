//
// Created by thijs on 17-12-18.
//

#include "SideAttacker.h"

namespace rtt {
namespace ai {

std::vector<std::shared_ptr<roboteam_msgs::WorldRobot>> SideAttacker::robotsPositioning = {};

SideAttacker::SideAttacker(string name, bt::Blackboard::Ptr blackboard)
    :Skill(std::move(name), std::move(blackboard)) {
}

void SideAttacker::onInitialize() {
    for (auto & i : robotsPositioning) {
        if (i->id == robot->id) {
            return;
        }
    } // TODO use std::find here, love best teammate ever

    robotsPositioning.push_back(robot);
}


/// Get an update on the skill
bt::Node::Status SideAttacker::onUpdate() {
    switch (currentProgress) {
        case DEFAULTING: {
            bool isIn = false;
            for (auto & i : robotsPositioning) {
                if (i->id == robot->id) {
                    isIn = true;
                }
            }

            if (!isIn) return Status::Running;

            targetPos = getDefaultLocation();

            if ((targetPos - robot->pos).length() < DEFAULT_DISTANCE_MARGIN) {
                currentProgress = OPTIMIZING;
            }

            break;
        }
        case OPTIMIZING: {
            //targetPos = coach::g_offensiveCoach.calculatePositionForRobot(robot);
            break;
        }
    }

    auto newPosition = goToPos.getPosVelAngle(robot, targetPos);
    Vector2 velocity = newPosition.vel;
    velocity = control::ControlUtils::velocityLimiter(velocity);
    command.x_vel = static_cast<float>(velocity.x);
    command.y_vel = static_cast<float>(velocity.y);
    command.w = static_cast<float>(newPosition.angle);

    publishRobotCommand();
    return Status::Running;
}

Vector2 SideAttacker::getDefaultLocation() {
    auto field = Field::get_field();

    std::vector<Vector2> targetLocations = coach::g_offensiveCoach.getDefaultLocations(robotsPositioning.size());
    std::vector<Vector2> robotLocations;

    for (auto & i : robotsPositioning) {
        robotLocations.emplace_back(i->pos);
    }

    auto shortestDistances = control::ControlUtils::calculateClosestPathsFromTwoSetsOfPoints(robotLocations, targetLocations);

    for (unsigned long i = 0; i<robotsPositioning.size(); i++) {
        if (robotsPositioning.at(i)->id == robot->id) {
            return shortestDistances.at(i).second;
        }
    }

    return {0, 0};
}

void SideAttacker::onTerminate(Status s) {
    command.w = static_cast<float>(deltaPos.angle());
    command.x_vel = 0;
    command.y_vel = 0;
    publishRobotCommand();
    coach::g_offensiveCoach.releaseRobot(robot->id);
}

} // ai
} // rtt