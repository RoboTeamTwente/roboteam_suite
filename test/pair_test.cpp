#include "utils.hpp"

TEST(ServerTests, websocket_test) {
    Server s{};

    std::thread t{ run_server, &s };
    sleep_ms(1000);

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
        std::cerr << "Received: Module State" << std::endl;
    });
    socket.start();
    sleep_ms(1000);
    ai.write(get_simple_module_state());
    while (!received)
        ;
    s.stop();
    t.join();
    ASSERT_TRUE(received);
}

TEST(ServerTests, interface_test) {
    Server s{};
    std::thread t{ run_server, &s };
    sleep_ms(1000);
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
        socket.sendBinary(get_ui_settings().SerializeAsString());
    });
    socket.start();
    sleep_ms(1000);
    ai.write(get_simple_module_state());
    ASSERT_TRUE(ai.read_next<proto::UiSettings>(false).is_ok());
    s.stop();
    t.join();
}
