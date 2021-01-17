#ifndef __INTERFACE_HPP__
#define __INTERFACE_HPP__

#include <ixwebsocket/IXWebSocketServer.h>

#include "type_traits.hpp"

namespace rtt::central {
    template <size_t port>
    struct Interface {
        ix::WebSocketServer server{ port };
        std::function<void(proto::UiSettings)> functor;

        void run(std::function<void(proto::UiSettings)>&& functor) {
            this->functor = functor;
            server.setOnClientMessageCallback(
                [this](std::shared_ptr<ix::ConnectionState> connectionState,
                       ix::WebSocket& webSocket,
                       const ix::WebSocketMessagePtr& msg) {
                    switch (msg->type) {
                        case ix::WebSocketMessageType::Open:
                            std::cout << "New incoming interface connection: " << connectionState->getId() << " (ping: " << webSocket.getPingInterval() << ")" << std::endl;
                            break;
                        case ix::WebSocketMessageType::Message:
                            handle_incoming(connectionState, webSocket, msg);
                            break;
                        case ix::WebSocketMessageType::Close:
                            std::cout << "Interface connection closed" << std::endl;
                            break;
                        case ix::WebSocketMessageType::Error:
                            std::cout << "Interface connection error" << std::endl;
                            break;
                        case ix::WebSocketMessageType::Ping:
                        case ix::WebSocketMessageType::Pong:
                        case ix::WebSocketMessageType::Fragment:
                        default:
                            break;
                    }
                });

            server.listen();
            server.start();
        }

        void handle_incoming(std::shared_ptr<ix::ConnectionState>, ix::WebSocket&, const ix::WebSocketMessagePtr& msg) {
            proto::UiSettings data;
            if (!data.ParseFromString(msg->str)) {
                std::cerr << "Something else than proto::UiSettings has been received by central from ui" << std::endl;
                return;
            }
            functor(data);
        }

        template <typename T>
        void write(T const& s) {
            static_assert(type_traits::is_serializable_v<T>, "T is not serializable to string in Connection::write()");
            std::string out;
            s.SerializeToString(&out);
            auto clients = server.getClients();
            for (auto const& each : clients) {
                each->sendBinary(out);
            }
        }

        void stop() {
            server.stop();
        }
    };
}  // namespace rtt::central

#endif