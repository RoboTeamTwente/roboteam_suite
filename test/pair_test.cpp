#include <roboteam_utils/networking/include/Pair.hpp>

#include "server.hpp"
#include <iostream>
using namespace rtt::central;

void run_server() {
    Server s{};
    s.run();
}

int main() {
    std::thread t{run_server};
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "1\n";
    rtt::networking::PairReceiver<16970> ai{};
    rtt::networking::PairReceiver<16971> interface{};
    std::cout << "2\n";
    proto::ModuleState ms;
    std::cout << "3\n";
    ai.write(ms);
    std::cout << "4\n";
    // false because actually wait.
    auto result = interface.read_next<proto::ModuleState>(false);
    std::cout << "5\n";
    std::cout << "ok? " << result.is_ok() << std::endl;
    std::cout << "6\n";
    t.join();
}