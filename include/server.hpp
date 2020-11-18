#ifndef __SERVER_HPP__
#define __SERVER_HPP__

#include <roboteam_proto/State.pb.h>
#include <stx/option.h>

#include <thread>
#include <vector>

#include "connection.hpp"
#include "module.hpp"

namespace rtt::central {

    struct Server {
        /**
         * @brief These are the modules connected.
         * They merely _receive_ data, they never send any, apart from inital handshake.
         */
        std::vector<Module> modules{};

        // Why couldn't I use rust again?...
        std::mutex _this_mutex;

        /**
         * @brief There is a direct conneciton to the AI and the interface.
         * Both are readwrite and continuously read and written to.
         */
        Connection<ConnectionType::READWRITE> roboteam_ai;
        Connection<ConnectionType::READWRITE> roboteam_interface;

        std::thread ai_thread;
        std::thread interface_thread;

        stx::Option<proto::State> current_ai_state;

        void handle_roboteam_ai() {
            // read incoming data and forward to modules.
            while (true) {

                roboteam_ai.read_next<proto::State>().match()
            }
        }

        void handle_interface_thread() {
            // Possibly spawn 2 threads? one for reading one for writing? not sure.
        }

        void run() {
            ai_thread = std::thread([this]() {
                this->handle_roboteam_ai();
            });
            while (true) {
                std::cout << "Looped..." << std::endl;
            }
        }
    };

}  // namespace rtt::central
#endif