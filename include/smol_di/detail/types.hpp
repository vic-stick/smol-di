#pragma once

#include <meta>

namespace smol_di {
template <typename Service, typename Implementation>
struct Binding {
    using service_type = Service;
    using implementation_type = Implementation;

    static constexpr auto service_info = ^^Service;
    static constexpr auto implementation_info = ^^Implementation;
};

} // namespace smol_di

namespace smol_di::detail {

enum class Lifetime { Singleton };
enum class DependencyKind { Value, LValueReference };

template <std::meta::info Service, std::meta::info Implementation,
          Lifetime Value>
struct Registration {
    using service_type = [:Service:];
    using implementation_type = [:Implementation:];
    static constexpr auto service_info = Service;
    static constexpr auto implementation_info = Implementation;
    static constexpr auto lifetime = Value;
};

template <std::meta::info... Types> struct type_list {};
template <typename... Dependencies> struct dependency_types {};
template <typename... Registrations> struct registration_types {};
template <typename... Bindings> struct binding_types {};

template <std::meta::info Type, DependencyKind Kind> struct Dependency {
    using type = [:Type:];
    static constexpr auto type_info = Type;
    static constexpr auto kind = Kind;
};
} // namespace smol_di::detail
