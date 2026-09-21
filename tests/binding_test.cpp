#include "smol_di/smol_di.hpp"

#include <gtest/gtest.h>

struct IUserRepository {
    virtual ~IUserRepository() = default;
};
struct UserRepository : IUserRepository {
    UserRepository() {}
};

struct IFoo {
    virtual ~IFoo() = default;
};
struct Foo : IFoo {};
struct IBar {
    virtual ~IBar() = default;
};
struct Bar : IBar {};
struct Database {};

using RepositoryBinding = smol_di::Binding<^^IUserRepository, ^^UserRepository>;

constexpr auto implementation =
    smol_di::detail::implementation_for<^^IUserRepository, RepositoryBinding>();
static_assert(implementation == ^^UserRepository);

using FooBinding = smol_di::Binding<^^IFoo, ^^Foo>;
using BarBinding = smol_di::Binding<^^IBar, ^^Bar>;

static_assert(smol_di::contains_binding<^^IFoo, FooBinding, BarBinding>());
static_assert(smol_di::contains_binding<^^IBar, FooBinding, BarBinding>());
static_assert(!smol_di::contains_binding<^^Database, FooBinding, BarBinding>());
static_assert(smol_di::validate_bindings<FooBinding, BarBinding>());

using DuplicateFooBinding = smol_di::Binding<^^IFoo, ^^Foo>;
static_assert(!smol_di::validate_bindings<FooBinding, DuplicateFooBinding>());

using ValidBinding = smol_di::Binding<^^IUserRepository, ^^UserRepository>;
using InvalidBinding = smol_di::Binding<^^IUserRepository, ^^Database>;

static_assert(smol_di::valid_binding<ValidBinding>());
static_assert(!smol_di::valid_binding<InvalidBinding>());

TEST(BindingTest, ResolvesImplementationAtCompileTime) { SUCCEED(); }
