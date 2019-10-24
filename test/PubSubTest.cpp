#include <gtest/gtest.h>
#include "roboteam_proto/Publisher.h"
#include "roboteam_proto/Subscriber.h"
#include "roboteam_proto/RobotCommand.pb.h"
#include "roboteam_utils/Timer.h"

bool messageReceivedSuccesFully = false;
double receivedTime = 0;

void handleRobotCommand(roboteam_proto::RobotCommand & robot_command) {
  EXPECT_EQ(robot_command.geneva_state(), 4);
  receivedTime = robot_command.id();
  messageReceivedSuccesFully = true;
}

TEST(PubSubTest, function_subscription) {
  auto sub = std::make_shared<roboteam_proto::Subscriber>("tcp://127.0.0.1:5555", "dummy_robotcommand_topic", &handleRobotCommand);
  auto pub = std::make_shared<roboteam_proto::Publisher>("tcp://127.0.0.1:5555");

  roboteam_proto::RobotCommand cmd;
  cmd.set_geneva_state(4);

  auto reference_time = roboteam_utils::Timer::getCurrentTime().count();
  while(!messageReceivedSuccesFully) {
    auto now = roboteam_utils::Timer::getCurrentTime().count();
    if (now > reference_time + 1000) {
      ASSERT_TRUE(false); // it takes too long. test should fail instead of deadlock
    }
    cmd.set_id(roboteam_utils::Timer::getCurrentTime().count()); // we store the time in the id
    pub->send("dummy_robotcommand_topic", cmd.SerializeAsString());
  }

  // the communication should be fast (<5ms)
  EXPECT_LE( roboteam_utils::Timer::getCurrentTime().count() - receivedTime,  5);
}


TEST(PubSubTest, method_subscription) {

  struct Dummy {
    roboteam_proto::RobotCommand cmd;
    std::shared_ptr<roboteam_proto::Subscriber> sub;
    bool got_command = false;
    int receivedTime = 0;

    Dummy() {
      sub = std::make_shared<roboteam_proto::Subscriber>("tcp://127.0.0.1:5555", "dummy_robotcommand_topic", &Dummy::handle_message, this);
    }

    void handle_message(roboteam_proto::RobotCommand & robotcommand) {
      cmd = robotcommand;
      receivedTime = robotcommand.id();
      got_command = true;
    }
  };

  Dummy dummy;

  auto pub = std::make_shared<roboteam_proto::Publisher>("tcp://127.0.0.1:5555");
  roboteam_proto::RobotCommand cmd;
  cmd.set_geneva_state(2);

  auto reference_time = roboteam_utils::Timer::getCurrentTime().count();
  while(!dummy.got_command) {
    auto now = roboteam_utils::Timer::getCurrentTime().count();
    if (now > reference_time + 1000) {
      ASSERT_TRUE(false); // it takes too long. test should fail instead of deadlock
    }
    cmd.set_id(roboteam_utils::Timer::getCurrentTime().count()); // we store the time in the id
    pub->send("dummy_robotcommand_topic", cmd.SerializeAsString());
  }

  // the communication should be fast (<5ms)
  EXPECT_LE( roboteam_utils::Timer::getCurrentTime().count() - dummy.receivedTime,  5);
  EXPECT_EQ(dummy.cmd.geneva_state(), 2);
}