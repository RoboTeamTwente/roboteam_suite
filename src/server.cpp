#include "server.hpp"

namespace rtt::central {

    Server::Server()
        : modules(Mutex<ModuleHandler>{ &this->module_handshakes }) {
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
                    [](std::string&& err) { std::cout << err << std::endl; });
        }
    }

    void Server::handle_success_state_read(proto::State ok) {
        std::cout << "Ok packet received: " << ok.ball_camera_world().id() << std::endl;
        // send it to the interface
        roboteam_interface.acquire()->write(ok);
        // forward this state to all modules.
        modules.acquire()->broadcast(ok);
        *current_ai_state.acquire() = stx::Some(std::move(ok));
    }

    void Server::handle_interface() {
        // spawn a thread for reading -> when reading was received update
        // this->current_settings?
        // only needs for reading, the only writing the interface gets is whatever the AI sends.
    }

    void Server::run() {
        new (&ai_thread) Mutex{ std::thread([this]() {
            this->handle_roboteam_ai();
        }) };

        new (&interface_thread) Mutex{ std::thread([this]() {
            this->handle_interface();
        }) };

        while (true) {
            std::cout << "Looped..." << std::endl;
        }

        ai_thread.acquire()->join();
        interface_thread.acquire()->join();
    }
}  // namespace rtt::central