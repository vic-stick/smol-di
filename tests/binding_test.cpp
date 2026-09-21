#include "smol_di/smol_di.hpp"

#include <gtest/gtest.h>

struct IUserRepository {
    virtual ~IUserRepository() = default;
};
struct UserRepository : IUserRepository {
    UserRepository() {}
};

using RepositoryBinding = smol_di::Binding<^^IUserRepository, ^^UserRepository>;

constexpr auto implementation =
    smol_di::detail::implementation_for<^^IUserRepository, RepositoryBinding>();
static_assert(implementation == ^^UserRepository);

TEST(BindingTest, ResolvesImplementationAtCompileTime) { SUCCEED(); }
