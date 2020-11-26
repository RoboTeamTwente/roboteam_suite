#ifndef __CONNECTION_HPP__
#define __CONNECTION_HPP__
// #define ZMQ_BUILD_DRAFT_API	1
#include <roboteam_proto/State.pb.h>
#include <stx/result.h>

#include <zmqpp/zmqpp.hpp>

#include "type_traits.hpp"

namespace rtt::central {

    template <zmqpp::socket_type ct, size_t port>
    struct Connection {
        zmqpp::context context;
        zmqpp::socket socket;

        Connection()
            : socket{ context, ct } {
            socket.bind("tcp://*:" + std::to_string(port));
        }

        template <typename T>
        stx::Result<T, std::string> read_next() {
            static_assert(type_traits::is_serializable_v<T>, "T is not serializable to string in Connection::write()");
            zmqpp::message msg;
            std::string data{};
            if (!socket.receive(msg, true)) {
                return stx::Err(std::move(data));
            }
            msg >> data;
            T object;
            auto succ = object.ParseFromString(data);
            if (succ) {
                return stx::Ok(std::move(object));
            }
            return stx::Err(std::move(data));
        }

        template <typename T>
        void write(T const& s) {
            static_assert(type_traits::is_serializable_v<T>, "T is not serializable to string in Connection::write()");
            std::string out;
            s.SerializeToString(&out);
            socket.send(out);
        }

        bool is_ok() {
            return static_cast<bool>(socket);
        }
    };

}  // namespace rtt::central

#endif