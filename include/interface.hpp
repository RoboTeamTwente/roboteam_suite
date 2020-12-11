#ifndef __INTERFACE_HPP__
#define __INTERFACE_HPP__

#include <uWebSockets/App.h>

#include "type_traits.hpp"

namespace rtt::central {
    template <size_t port>
    struct Interface {
        struct SocketData {};
        uWS::App app;
        std::function<void(proto::UiSettings)> functor;

        Mutex<std::vector<uWS::WebSocket<false, true>*>> _connected;

        void run(std::function<void(proto::UiSettings)>&& functor) {
            this->functor = functor;
            uWS::TemplatedApp<false>::WebSocketBehavior general_catch = {
                .open = [this](auto* ws) {
                    auto connected = _connected.acquire();
                    connected->emplace_back(ws); },
                .message = [this](auto* ws, std::string_view message, uWS::OpCode opCode) {
                    this->handle_incoming(ws, message, opCode);
                },
                .close = [this](auto* ws, int /*code*/, std::string_view /*message*/) {
                    auto connected = _connected.acquire();
                    connected->erase(std::remove(connected->begin(), connected->end(), ws), connected->end()); }
            };
            app.ws<SocketData>("/*", std::move(general_catch))
                .listen(port, [](auto*){});
        }

        void handle_incoming([[maybe_unused]] uWS::WebSocket<false, true>* ws, std::string_view message, [[maybe_unused]] uWS::OpCode opCode) {
            proto::UiSettings data;
            if (!data.ParseFromArray(message.data(), message.size())) {
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
            auto connected = _connected.acquire();
            for (auto const& each : *connected) {
                each->send(out);
            }
        }
    };
}  // namespace rtt::central

#endif