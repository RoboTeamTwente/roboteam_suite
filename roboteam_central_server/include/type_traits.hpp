#ifndef __TYPE_TRAITS_HPP__
#define __TYPE_TRAITS_HPP__

#include <type_traits>
#include <experimental/type_traits>

namespace rtt::central::type_traits {
    template<class T>
    using has_serializable = 
        decltype(std::declval<T&>().SerializeToString(std::declval<std::string*>()));

    template <typename T>
    constexpr static inline bool is_serializable_v = std::experimental::is_detected_exact_v<bool, has_serializable, T>;
}  // namespace rtt::central::type_traits

#endif