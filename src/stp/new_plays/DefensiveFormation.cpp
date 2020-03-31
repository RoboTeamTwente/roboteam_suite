//
// Created by timo on 3/27/20.
//

#include "stp/new_plays/DefensiveFormation.h"
#include "stp/invariants/WeHaveBallInvariant.h"

#include "stp/new_roles/Formation.h"

namespace rtt::ai::stp::play {

DefensiveFormation::DefensiveFormation(std::string playName) : Play(std::move(playName)) {
    // TODO: decide invariants
    invariants.clear();
    invariants.emplace_back(std::make_unique<invariant::WeHaveBallInvariant>());

    roles = std::array<std::unique_ptr<Role>, rtt::ai::Constants::ROBOT_COUNT()>{
        std::make_unique<role::Formation>(role::Formation("keeper")),      std::make_unique<role::Formation>(role::Formation("defender_0")),
        std::make_unique<role::Formation>(role::Formation("defender_1")),  std::make_unique<role::Formation>(role::Formation("defender_2")),
        std::make_unique<role::Formation>(role::Formation("defender_3")),  std::make_unique<role::Formation>(role::Formation("mid_field_0")),
        std::make_unique<role::Formation>(role::Formation("mid_field_1")), std::make_unique<role::Formation>(role::Formation("mid_field_2")),
        std::make_unique<role::Formation>(role::Formation("offender_0")),  std::make_unique<role::Formation>(role::Formation("offender_1")),
        std::make_unique<role::Formation>(role::Formation("offender_2"))};
}

uint8_t DefensiveFormation::score(world_new::World* world) noexcept { return 20; }

void DefensiveFormation::calculateInfoForRoles() noexcept {
    // TODO: TUNE these positions could probably be a bit better once we decide how we want to play
    if (stpInfos.find("keeper") != stpInfos.end()) stpInfos["keeper"].setPositionToMoveTo(Vector2{-4.5, 0});
    if (stpInfos.find("defender_0") != stpInfos.end()) stpInfos["defender_0"].setPositionToMoveTo(Vector2{-3, 4});
    if (stpInfos.find("defender_1") != stpInfos.end()) stpInfos["defender_1"].setPositionToMoveTo(Vector2{-3, 1});
    if (stpInfos.find("defender_2") != stpInfos.end()) stpInfos["defender_2"].setPositionToMoveTo(Vector2{-3, -1});
    if (stpInfos.find("defender_3") != stpInfos.end()) stpInfos["defender_3"].setPositionToMoveTo(Vector2{-3, -4});
    if (stpInfos.find("mid_field_0") != stpInfos.end()) stpInfos["mid_field_0"].setPositionToMoveTo(Vector2{-2, 3});
    if (stpInfos.find("mid_field_1") != stpInfos.end()) stpInfos["mid_field_1"].setPositionToMoveTo(Vector2{-2, 0});
    if (stpInfos.find("mid_field_2") != stpInfos.end()) stpInfos["mid_field_2"].setPositionToMoveTo(Vector2{-2, -3});
    if (stpInfos.find("offender_0") != stpInfos.end()) stpInfos["offender_0"].setPositionToMoveTo(Vector2{-1, 4});
    if (stpInfos.find("offender_1") != stpInfos.end()) stpInfos["offender_1"].setPositionToMoveTo(Vector2{-1, 0});
    if (stpInfos.find("offender_2") != stpInfos.end()) stpInfos["offender_2"].setPositionToMoveTo(Vector2{-1, -4});
}

bool DefensiveFormation::isValidPlayToStart(world_new::World* world) noexcept { return true; }

bool DefensiveFormation::shouldRoleSkipEndTactic() { return false; }

Dealer::FlagMap DefensiveFormation::decideRoleFlags() const noexcept {
    Dealer::FlagMap flagMap;
    Dealer::DealerFlag closeToBallFlag(DealerFlagTitle::CLOSE_TO_BALL, DealerFlagPriority::HIGH_PRIORITY);
    Dealer::DealerFlag closeToTheirGoalFlag(DealerFlagTitle::CLOSE_TO_THEIR_GOAL, DealerFlagPriority::MEDIUM_PRIORITY);
    Dealer::DealerFlag notImportant(DealerFlagTitle::CLOSE_TO_OUR_GOAL, DealerFlagPriority::LOW_PRIORITY);

    flagMap.insert({"keeper", {closeToBallFlag}});
    flagMap.insert({"defender_0", {closeToTheirGoalFlag}});
    flagMap.insert({"defender_1", {notImportant}});
    flagMap.insert({"defender_2", {closeToTheirGoalFlag}});
    flagMap.insert({"defender_3", {closeToBallFlag}});
    flagMap.insert({"mid_field_0", {closeToTheirGoalFlag, closeToBallFlag}});
    flagMap.insert({"mid_field_1", {closeToBallFlag}});
    flagMap.insert({"mid_field_2", {closeToTheirGoalFlag}});
    flagMap.insert({"offender_0", {closeToTheirGoalFlag, closeToBallFlag}});
    flagMap.insert({"offender_1", {closeToBallFlag}});
    flagMap.insert({"offender_2", {closeToTheirGoalFlag}});

    return flagMap;
}
}  // namespace rtt::ai::stp::play