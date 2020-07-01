//
// Created by timovdk on 3/12/20.
//

#include "stp/new_tactics/KickAtPos.h"

#include <control/ControlUtils.h>
#include <stp/new_skills/Kick.h>
#include <stp/new_skills/Rotate.h>

namespace rtt::ai::stp::tactic {

KickAtPos::KickAtPos() {
    // Create state machine of skills and initialize first skill
    skills = rtt::collections::state_machine<Skill, Status, StpInfo>{skill::Rotate(), skill::Kick()};
}

void KickAtPos::onInitialize() noexcept {}

void KickAtPos::onUpdate(Status const &status) noexcept {}

void KickAtPos::onTerminate() noexcept {
    // Call terminate on all skills
    for (auto &x : skills) {
        x->terminate();
    }
}

std::optional<StpInfo> KickAtPos::calculateInfoForSkill(StpInfo const &info) noexcept {
    StpInfo skillStpInfo = info;

    if (!skillStpInfo.getPositionToShootAt() || !skillStpInfo.getRobot() || !skillStpInfo.getBall()) return std::nullopt;

    // Calculate the angle the robot needs to aim
    double angleToTarget = (info.getPositionToShootAt().value() - info.getRobot().value()->getPos()).angle();
    skillStpInfo.setAngle(angleToTarget);

    // Calculate the distance and the kick force
    double distanceBallToTarget = (info.getBall()->get()->getPos() - info.getPositionToShootAt().value()).length();
    skillStpInfo.setKickChipVelocity(control::ControlUtils::determineKickForce(distanceBallToTarget, skillStpInfo.getKickChipType()));

    // When the angle is not within the margin, dribble so we don't lose the ball while rotating
    double errorMargin = stp::control_constants::GO_TO_POS_ANGLE_ERROR_MARGIN * M_PI;
    skillStpInfo.setDribblerSpeed(50);

    return skillStpInfo;
}

bool KickAtPos::isEndTactic() noexcept {
    // This is not an end tactic
    return false;
}

bool KickAtPos::isTacticFailing(const StpInfo &info) noexcept {
    // Fail tactic if:
    // robot doesn't have the ball or if there is no shootTarget
    return !info.getRobot()->hasBall() || !info.getPositionToShootAt();
}

bool KickAtPos::shouldTacticReset(const StpInfo &info) noexcept {
    // Reset when angle is wrong outside of the rotate skill, reset to rotate again
    if (skills.current_num() != 0) {
        double errorMargin = stp::control_constants::GO_TO_POS_ANGLE_ERROR_MARGIN * M_PI;
        return info.getRobot().value()->getAngle().shortestAngleDiff(info.getAngle()) > errorMargin;
    }
    return false;
}

const char *KickAtPos::getName() { return "Kick At Pos"; }

}  // namespace rtt::ai::stp::tactic
