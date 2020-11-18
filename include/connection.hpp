#ifndef __CONNECTION_HPP__
#define __CONNECTION_HPP__

#include <roboteam_proto/State.pb.h>

#include <stx/result.h>

namespace rtt::central {

    enum class ConnectionType {
        READ = 0,
        WRITE = 1,
        READWRITE = READ | WRITE,
    };

    template <ConnectionType ct>
    struct Connection {
        template <typename T>
        stx::Result<T, std::string> read_next() {
            return stx::Err("");
        } 
    };

}  // namespace rtt::central

#endif