//
// Created by timovdk on 3/13/20.
//

#include "stp/new_tactics/ChipAtPos.h"

#include <control/ControlUtils.h>
#include <stp/new_skills/Chip.h>
#include <stp/new_skills/Rotate.h>

namespace rtt::ai::stp::tactic {

ChipAtPos::ChipAtPos() {
    // Create state machine of skills and initialize first skill
    skills = rtt::collections::state_machine<Skill, Status, StpInfo>{skill::Rotate(), skill::Chip()};
}

void ChipAtPos::onInitialize() noexcept {}

void ChipAtPos::onUpdate(Status const &status) noexcept {}

void ChipAtPos::onTerminate() noexcept {
    // Call terminate on all skills
    for (auto &x : skills) {
        x->terminate();
    }
}

std::optional<StpInfo> ChipAtPos::calculateInfoForSkill(StpInfo const &info) noexcept {
    StpInfo skillStpInfo = info;

    if (!skillStpInfo.getPositionToShootAt() || !skillStpInfo.getRobot() || !skillStpInfo.getBall()) return std::nullopt;

    // Calculate the angle the robot needs to aim
    double angleToTarget = (info.getPositionToShootAt().value() - info.getRobot().value()->getPos()).angle();
    skillStpInfo.setAngle(angleToTarget);

    // Calculate the distance and the chip force
    double distanceBallToTarget = (info.getBall()->get()->getPos() - info.getPositionToShootAt().value()).length();
    skillStpInfo.setKickChipVelocity(control::ControlUtils::determineChipForce(distanceBallToTarget, skillStpInfo.getKickChipType()));

    // When rotating, we need to dribble to keep the ball, but when chipping we don't
    if (skills.current_num() == 0) {
        skillStpInfo.setDribblerSpeed(30);
    }

    return skillStpInfo;
}

bool ChipAtPos::isEndTactic() noexcept {
    // This is not an end tactic
    return false;
}

bool ChipAtPos::isTacticFailing(const StpInfo &info) noexcept {
    // Fail tactic if:
    // robot doesn't have the ball or if there is no shootTarget
    // But only check when we are not chipping
    if (skills.current_num() != 1) {
        return !info.getRobot()->hasBall() || !info.getPositionToShootAt();
    }
    return false;
}

bool ChipAtPos::shouldTacticReset(const StpInfo &info) noexcept {
    // Reset when angle is wrong outside of the rotate skill, reset to rotate again
    if (skills.current_num() != 0) {
        double errorMargin = stp::control_constants::GO_TO_POS_ANGLE_ERROR_MARGIN * M_PI;
        return info.getRobot().value()->getAngle().shortestAngleDiff(info.getAngle()) > errorMargin;
    }
    return false;
}

const char *ChipAtPos::getName() { return "Chip At Pos"; }

}  // namespace rtt::ai::stp::tactic
