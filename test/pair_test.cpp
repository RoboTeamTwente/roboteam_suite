#include <gtest/gtest.h>
#include <ixwebsocket/IXWebSocket.h>

#include <iostream>
#include <roboteam_utils/networking/include/Pair.hpp>

#include "server.hpp"
using namespace rtt::central;

void run_server(Server* s) {
    s->run();
}

TEST(ServerTests, websocket_test) {
    Server s{};

    std::thread t{ run_server, &s };
    std::this_thread::sleep_for(std::chrono::seconds(1));

    rtt::networking::PairReceiver<16970> ai{};

    ix::WebSocket socket{};
    socket.setUrl("ws://localhost:16971/");
    bool received = false;
    socket.setOnMessageCallback([&](ix::WebSocketMessagePtr const& msg) {
        // never received withtype == Message
        std::cout << "Message type " << (int)msg->type << std::endl;
        if (msg->type != ix::WebSocketMessageType::Message) {
            // std::cout << ptr->errorInfo.decompressionError << "\n"
            //         << ptr->errorInfo.http_status << "\n"
            //         << ptr->errorInfo.reason << "\n"
            //         << ptr->errorInfo.retries << "\n"
            //         << ptr->errorInfo.wait_time << "\n";
            return;
        }
        proto::ModuleState data;
        ASSERT_TRUE(data.ParseFromString(msg->str));
        received = true;
        std::cerr << "Received: " << data.GetDescriptor()->DebugString() << std::endl;
    });
    socket.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    proto::ModuleState ms;
    auto hs = ms.add_handshakes();
    hs->set_name("test");
    ai.write(ms);
    while (!received);
    ASSERT_TRUE(received);
    s.stop();
    t.join();
}

TEST(ServerTests, interface_test) {
    Server s{};

    std::thread t{ run_server, &s };
    std::this_thread::sleep_for(std::chrono::seconds(1));

    rtt::networking::PairReceiver<16970> ai{};

    ix::WebSocket socket{};
    bool received = false;
    socket.setUrl("ws://localhost:16971/");
    socket.setOnMessageCallback([&socket, &received](ix::WebSocketMessagePtr const& msg) {
        std::cout << "Message type " << (int)msg->type << std::endl;
        if (msg->type != ix::WebSocketMessageType::Message) {
            // std::cout << ptr->errorInfo.decompressionError << "\n"
            //         << ptr->errorInfo.http_status << "\n"
            //         << ptr->errorInfo.reason << "\n"
            //         << ptr->errorInfo.retries << "\n"
            //         << ptr->errorInfo.wait_time << "\n";
            return;
        }
        proto::ModuleState data;
        ASSERT_TRUE(data.ParseFromString(msg->str));
        received = true;
        std::cerr << "Received: " << data.GetDescriptor()->DebugString() << std::endl;
        proto::UiSettings settings;
        proto::PossibleUiValue ui_value{};
        ui_value.set_allocated_text_value(new std::string{ "test123" });
        (*settings.mutable_ui_values())["interface_test"] = ui_value;
        socket.sendBinary(settings.SerializeAsString());
    });
    socket.start();
    proto::ModuleState ms;
    auto hs = ms.add_handshakes();
    hs->set_name("test");
    std::this_thread::sleep_for(std::chrono::seconds(1));
    ai.write(ms);
    // false because actually wait.
    auto result = ai.read_next<proto::UiSettings>(false);
    std::cout << "ok? " << result.is_ok() << std::endl;
    ASSERT_TRUE(result.is_ok());
    s.stop();
    t.join();
}