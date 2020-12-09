#include <gtest/gtest.h>

#include <iostream>
#include <roboteam_utils/networking/include/Pair.hpp>

#include "server.hpp"
using namespace rtt::central;

void run_server(Server* s) {
    s->run();
}

TEST(ServerTests, pair_test) {
    Server s{};
    std::thread t{ run_server, &s };
    std::this_thread::sleep_for(std::chrono::seconds(1));
    rtt::networking::PairReceiver<16970> ai{};
    rtt::networking::PairReceiver<16971> interface{};
    proto::ModuleState ms;
    ai.write(ms);
    // false because actually wait.
    auto result = interface.read_next<proto::ModuleState>(false);
    ASSERT_TRUE(result.is_ok());
    s.stop();
    t.join();
}