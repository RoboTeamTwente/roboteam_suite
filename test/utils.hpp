#ifndef __CENTRAL_UTILS_HPP__
#define __CENTRAL_UTILS_HPP__
#include <gtest/gtest.h>
#include <ixwebsocket/IXWebSocket.h>

#include <iostream>
#include <roboteam_utils/networking/include/Module.hpp>
#include <roboteam_utils/networking/include/Pair.hpp>

#include "server.hpp"
using namespace rtt::central;

void run_server(Server* s) {
    s->run();
}

proto::UiSettings get_ui_settings() {
    proto::UiSettings settings;
    proto::PossibleUiValue ui_value{};
    ui_value.set_allocated_text_value(new std::string{ "test123" });
    (*settings.mutable_ui_values())["interface_test"] = ui_value;
    return settings;
}

proto::ModuleState get_simple_module_state() {
    proto::ModuleState ms;
    auto hs = ms.add_handshakes();
    hs->set_name("test");
    return ms;
}
void sleep_ms(int ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

#endif