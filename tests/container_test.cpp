#include "smol_di/smol_di.hpp"

#include <gtest/gtest.h>

struct RepositoryConfig {};
struct IUserRepository {
    virtual ~IUserRepository() = default;
};
struct UserRepository : IUserRepository {
    explicit UserRepository(RepositoryConfig &config) : config(config) {}
    RepositoryConfig &config;
};
struct UserService {
    explicit UserService(IUserRepository &repository)
        : repository(repository) {}
    IUserRepository &repository;
};

TEST(ContainerTest, InjectsBoundImplementation) {
    using RepositoryBinding =
        smol_di::Binding<IUserRepository, UserRepository>;
    auto app = smol_di::create_container<UserService, RepositoryBinding>();

    auto &service = app.get<UserService>();
    auto &repository = app.get<IUserRepository>();

    EXPECT_EQ(&service.repository, &repository);
}
