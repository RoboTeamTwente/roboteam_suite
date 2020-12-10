#include "server.hpp"

namespace rtt::central {

    Server::Server()
        : modules{ &this->module_handshakes } {
    }

    void Server::handle_roboteam_ai() {
        // read incoming data and forward to modules.
        while (_run.load()) {
            auto ai = roboteam_ai.acquire();
            auto setting_message = current_settings.acquire()->clone();
            if (setting_message.is_some()) {
                ai->write(std::move(setting_message).unwrap());
            }

            ai->read_next<proto::ModuleState>()
                .match(
                    [this](proto::ModuleState&& ok) { this->handle_success_state_read(ok); },
                    [](std::string&& err) { 
                        if (err.size()) {
                            std::cout << "Error packet received" << std::endl;
                            std::cout << err << std::endl;
                     } });
            // std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }

    void Server::handle_success_state_read(proto::ModuleState ok) {
        std::cout << "Ok packet received: " << ok.state().ball_camera_world().id() << std::endl;
        // send it to the interface
        roboteam_interface.acquire()->write(ok);
        // forward this state to all modules.
        modules.broadcast(ok);
    }

    void Server::handle_interface() {
        while (_run.load()) {
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
            // sleep thread but somehow prevent busy-waiting?
            // implement scheduler?
            // setInterval()?
            // setTimeout()?
            // idk @rolf
        }
    }

    void Server::handle_modules() {
        modules.run(std::ref(this->_run));
    }

    void Server::run() {
        std::cout << "Constructing AI thread." << std::endl;
        new (&ai_thread) Mutex{ std::thread([this]() {
            this->handle_roboteam_ai();
        }) };

        std::cout << "Constructing UI thread." << std::endl;
        new (&interface_thread) Mutex{ std::thread([this]() {
            this->handle_interface();
        }) };

        std::cout << "Constructing Modules thread." << std::endl;
        new (&module_thread) Mutex{ std::thread([this]() {
            this->handle_modules();
        }) };

        ai_thread.acquire()->join();
        interface_thread.acquire()->join();
        module_thread.acquire()->join();
    }

    void Server::stop() {
        this->_run.store(false);
    }
}  // namespace rtt::central