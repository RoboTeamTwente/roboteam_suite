#ifndef __MODULE_HPP__
#define __MODULE_HPP__

#include <roboteam_proto/Handshake.pb.h>

#include "connection.hpp"
#include "mutex.hpp"

namespace rtt::central {

    struct ModuleHandler {
        ModuleHandler() = default;
        // zmqpp::socket_type::server
        // Send and read from many
        Mutex<Connection<zmqpp::socket_type::server, 16969>> conns;

        Mutex<std::thread> conns_threads;

        Mutex<std::vector<proto::Handshake>>* _handshake_vector;

        ModuleHandler(Mutex<std::vector<proto::Handshake>>* handshake_vector)
            : _handshake_vector{ handshake_vector } {
        }

        template <typename T>
        void broadcast(T const& data) {
            static_assert(type_traits::is_serializable_v<T>, "T is not serializable in ModuleHandler::broadcast()");
            if (_handshake_vector->acquire()->empty()) {
                return;
            }
            conns.acquire()->write(data);
        }

        void run() {
            while (true) {
                conns.acquire()->read_next<proto::Handshake>().match(
                    [this](proto::Handshake ok) {
                        // check whether maybe this module name is already included?
                        // if so -> drop it?
                        // handshake received, push to _handshake_vector
                        // Maybe change _handshake_vector to an std::unordered_map<std::string name, Handshake data> ?
                        _handshake_vector->acquire()->emplace_back(std::move(ok));
                    },
                    [](std::string&& err) {
                        if (err.size() == 0) {
                            return;
                        }
                        // something else than a valid Handshake was received
                        std::cout << err << std::endl;
                    }
                );
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        }
    };

}  // namespace rtt::central

#endif