#include "server.hpp"

namespace rtt::central {

    Server::Server()
        : modules{ &this->module_handshakes } {
    }

    void Server::handle_roboteam_ai() {
        // read incoming data and forward to modules.
        while (true) {
            auto ai = roboteam_ai.acquire();
            auto setting_message = current_settings.acquire()->clone();
            if (setting_message.is_some()) {
                ai->write(std::move(setting_message).unwrap());
            }

            ai->read_next<proto::State>()
                .match(
                    [this](proto::State&& ok) { this->handle_success_state_read(ok); },
                    [](std::string&& err) { 
                        if (err.size()) {
                            std::cout << err << std::endl;
                     } });
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }

    void Server::handle_success_state_read(proto::State ok) {
        std::cout << "Ok packet received: " << ok.ball_camera_world().id() << std::endl;
        // send it to the interface
        roboteam_interface.acquire()->write(ok);
        // forward this state to all modules.
        modules.broadcast(ok);
    }

    void Server::handle_interface() {
        while (true) {
            auto interface = roboteam_interface.acquire();
            interface->read_next<proto::UiSettings>()
                .match(
                    [this](proto::UiSettings&& ok) {
                        *this->current_settings.acquire() = stx::Some(std::move(ok));
                    },
                    [](std::string&& err) {
                        if (err.size() == 0) {
                            return;
                        }
                        std::cout << err << std::endl;
                    });
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }

    void Server::handle_modules() {
        modules.run();
    }

    void Server::run() {
        new (&ai_thread) Mutex{ std::thread([this]() {
            this->handle_roboteam_ai();
        }) };

        new (&interface_thread) Mutex{ std::thread([this]() {
            this->handle_interface();
        }) };

        new (&module_thread) Mutex{ std::thread([this]() {
            this->handle_modules();
        }) };

        ai_thread.acquire()->join();
        interface_thread.acquire()->join();
    }
}  // namespace rtt::central