#include <iostream>
#include "server.hpp"

int main(int, char**) {
    rtt::central::Server server{ };
    server.run();
}
