#pragma once

#include "smol_di/detail/container.hpp"

namespace smol_di {

template <typename Root, typename... Bindings> auto create_container() {
    return detail::make_container<Root, Bindings...>();
}

} // namespace smol_di
