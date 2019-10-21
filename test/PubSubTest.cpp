#include <gtest/gtest.h>
#include "roboteam_proto/Publisher.h"
#include "roboteam_proto/Subscriber.h"
#include "roboteam_proto/RobotCommand.pb.h"
#include "../../roboteam_utils/include/roboteam_utils/Timer.h"

bool messageReceivedSuccesFully = false;
double receivedTime = 0;

void handleRobotCommand(roboteam_proto::RobotCommand & robot_command) {
  EXPECT_EQ(robot_command.geneva_state(), 4);
  receivedTime = robot_command.id();
  messageReceivedSuccesFully = true;
}

TEST(PubSubTest, function_subscription) {
  auto sub = new roboteam_proto::Subscriber("tcp://127.0.0.1:5555", "dummy_robotcommand_topic", &handleRobotCommand);
  auto pub = new roboteam_proto::Publisher("tcp://127.0.0.1:5555");

  roboteam_proto::RobotCommand cmd;
  cmd.set_geneva_state(4);

  while(!messageReceivedSuccesFully) {
    cmd.set_id(roboteam_utils::Timer::getCurrentTime().count()); // we store the time in the id
    pub->send("dummy_robotcommand_topic", cmd.SerializeAsString());
  }

  // the communication should be super FAST (<5ms)
  ASSERT_LE( roboteam_utils::Timer::getCurrentTime().count() - receivedTime,  5);

  delete pub;
  delete sub;
}


TEST(PubSubTest, method_subscription) {

  struct Dummy {
    roboteam_proto::RobotCommand cmd;
    roboteam_proto::Subscriber * sub;
    bool got_command = false;
    int receivedTime = 0;

    Dummy() {
      sub = new roboteam_proto::Subscriber("tcp://127.0.0.1:5555", "dummy_robotcommand_topic", &Dummy::handle_message, this);
    }

    void handle_message(roboteam_proto::RobotCommand & robotcommand) {
      cmd = robotcommand;
      receivedTime = robotcommand.id();
      got_command = true;
    }

    ~Dummy() {
      delete sub;
    }
  };

  Dummy dummy;

  auto pub = new roboteam_proto::Publisher("tcp://127.0.0.1:5555");
  roboteam_proto::RobotCommand cmd;
  cmd.set_geneva_state(2);

  while(!dummy.got_command) {
    cmd.set_id(roboteam_utils::Timer::getCurrentTime().count()); // we store the time in the id
    pub->send("dummy_robotcommand_topic", cmd.SerializeAsString());
  }

  // the communication should be fast (<5ms)
ASSERT_LE( roboteam_utils::Timer::getCurrentTime().count() - dummy.receivedTime,  5);
  EXPECT_EQ(dummy.cmd.geneva_state(), 2);

  delete pub;
}