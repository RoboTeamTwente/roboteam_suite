#ifndef __SERVER_HPP__
#define __SERVER_HPP__

#include <roboteam_proto/Setting.pb.h>
#include <roboteam_proto/State.pb.h>
#include <stx/option.h>

#include <thread>
#include <vector>

#include "connection.hpp"
#include "module.hpp"
#include "mutex.hpp"
#include "utils.hpp"

namespace rtt::central {

    struct Server {
        /**
         * @brief These are the modules connected.
         * They merely _receive_ data, they never send any, apart from inital handshake.
         */
        Mutex<ModuleHandler> modules;  // ->write() broadcasts


        /**
         * @brief There is a direct conneciton to the AI and the interface.
         * Both are readwrite and continuously read and written to.
         */
        Mutex<Connection<zmqpp::socket_type::pair, 16970>> roboteam_ai;
        Mutex<Connection<zmqpp::socket_type::pair, 16971>> roboteam_interface;

        Mutex<std::thread> ai_thread;
        Mutex<std::thread> interface_thread;

        Mutex<stx::Option<proto::State>> current_ai_state;
        // placeholder type Setting
        Mutex<stx::Option<proto::Setting>> current_settings;

        void handle_roboteam_ai() {
            // read incoming data and forward to modules.
            while (true) {
                auto ai = roboteam_ai.acquire();
                auto setting_message = current_settings.acquire()->clone();
                if (setting_message.is_some()) {
                    ai->write(std::move(setting_message).unwrap());
                }
                
                auto const ai_packet = ai->read_next<proto::State>();
                ai_packet.match(
                    [this](proto::State ok) {
                        std::cout << "Ok packet received: " << ok.ball_camera_world().id() << std::endl;
                        // forward this state to all modules.
                        modules.acquire()->broadcast(ok);
                        roboteam_interface.acquire()->write(ok);
                        *current_ai_state.acquire() = stx::Some(std::move(ok));
                    },
                    [](std::string err) {
                        std::cout << err << std::endl;
                        // err is a string that's the message data.
                    });
            }
        }

        void handle_interface() {
            // Possibly spawn 2 threads? one for reading one for writing? not sure.
        }

        void run() {
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
    };

}  // namespace rtt::central
#endif