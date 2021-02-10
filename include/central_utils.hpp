#ifndef __UTILS_HPP__
#define __UTILS_HPP__

#include <stx/panic.h>

namespace rtt::central::utils {
    template <typename T>
    std::string proto_to_string(T const& data) {
        std::string out;
        if (data.SerializeToString(&out)) {
            return out;
        }
        stx::panic("`proto_to_string` received `data`, but `data` was not serializable.");
        return "";
    }
}

#endif