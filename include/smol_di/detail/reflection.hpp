#pragma once

#include "smol_di/detail/type_lists.hpp"

namespace smol_di::detail {
template <std::meta::info Param> consteval auto make_dependency() {
    constexpr auto parameter_type = std::meta::type_of(Param);
    constexpr auto type = std::meta::remove_reference(parameter_type);
    constexpr auto kind = std::meta::is_lvalue_reference_type(parameter_type)
                              ? DependencyKind::LValueReference
                              : DependencyKind::Value;
    return Dependency<type, kind>{};
}
template <auto... Params> struct dependency_builder {
    using type = dependency_types<decltype(make_dependency<Params>())...>;
};
template <typename R> consteval auto build_dependency_types(R range) {
    std::vector<std::meta::info> args;
    for (auto r : range)
        args.push_back(std::meta::reflect_constant(r));
    return std::meta::substitute(^^dependency_builder, args);
}
consteval auto get_constructor(std::meta::info type) {
    for (auto member :
         std::meta::members_of(type, std::meta::access_context::current()))
        if (std::meta::is_constructor(member))
            return member;
    return std::meta::info{};
}
template <std::meta::info Type> consteval auto dependency_list() {
    constexpr auto constructor = get_constructor(Type);
    constexpr auto builder =
        build_dependency_types(std::meta::parameters_of(constructor));
    using Builder = [:builder:];
    return typename Builder::type{};
}
} // namespace smol_di::detail
