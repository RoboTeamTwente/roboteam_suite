//
// Created by jessevw on 19.11.19.
//
#include <include/roboteam_ai/TreeHelper/RobotOutOfFieldHelper.h>
#include <include/roboteam_ai/bt/composites/Sequence.hpp>
#include <include/roboteam_ai/conditions/RobotOutside.h>
#include <include/roboteam_ai/skills/gotopos/GTPSpecial.h>
#include "bt/RoleDivider.h"
#include "bt/Role.h"
#include "skills/Attack.h"

namespace bt {
    RobotOutOfFieldHelper::RobotOutOfFieldHelper() {

    }
    std::shared_ptr<bt::Node> RobotOutOfFieldHelper::createRobotOutOfFieldHelper() {
        auto localbb = std::make_shared<bt::Blackboard>();
        auto robotOutsideBB = std::make_shared<bt::Blackboard>();

        robotOutsideBB->setString("type", "getBackIn");

        std::shared_ptr<bt::Sequence> seque = std::make_shared<bt::Sequence>();
        std::shared_ptr<rtt::ai::RobotOutside> robotOutside = std::make_shared<rtt::ai::RobotOutside>("RobotOutside", robotOutsideBB);
        std::shared_ptr<rtt::ai::GTPSpecial> gtp = std::make_shared<rtt::ai::GTPSpecial>("GoToPosMidfield", localbb);

        seque->addChild(robotOutside);
        seque->addChild(gtp);
        return seque;
    }
}