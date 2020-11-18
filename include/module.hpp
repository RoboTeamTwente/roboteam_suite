#ifndef __MODULE_HPP__
#define __MODULE_HPP__

#include "connection.hpp"
#include "mutex.hpp"

namespace rtt::central {

    struct ModuleHandler {
        // zmqpp::socket_type::xrequest
        Mutex<Connection<zmqpp::socket_type::xrequest, 16969>> conns{};

        ModuleHandler() {
        }

        template <typename T>
        void broadcast(T const& data) {
            static_assert(type_traits::is_serializable_v<T>, "T is not serializable in ModuleHandler::broadcast()");
            conns.acquire()->write(data);
        }
    };

}  // namespace rtt::central
#endif