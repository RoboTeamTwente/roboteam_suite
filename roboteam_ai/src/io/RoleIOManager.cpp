#include <roboteam_msgs/RoleFeedback.h>
#include "RoleIOManager.h"

RoleIOManager::RoleIOManager() {
  this->subscribeToWorldState();
  this->subscribeToRoleDirective();
}

void RoleIOManager::subscribeToRoleDirective() {
  nodeHandle.subscribe<roboteam_msgs::RoleDirective>(rtt::TOPIC_ROLE_DIRECTIVE, 1, &RoleIOManager::handleRoleDirective, this);
}

void RoleIOManager::handleRoleDirective(const roboteam_msgs::RoleDirectiveConstPtr &roleDirective) {
  this->roleDirective = * roleDirective;
}

roboteam_msgs::RoleDirective &RoleIOManager::getRoleDirective() {
  return this->roleDirective;
}
