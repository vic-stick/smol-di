#include "smol_di/smol_di.hpp"

#include <gtest/gtest.h>

struct RepositoryConfig {};

struct IUserRepository {
    virtual ~IUserRepository() = default;
    virtual RepositoryConfig &config() = 0;
};

struct UserRepository : IUserRepository {
    explicit UserRepository(RepositoryConfig &config) : config_(config) {}

    RepositoryConfig &config() override { return config_; }

  private:
    RepositoryConfig &config_;
};

struct UserService {
    explicit UserService(IUserRepository &repository) : repository(repository) {}

    IUserRepository &repository;
};

struct UserController {
    explicit UserController(UserService &service) : service(service) {}

    UserService &service;
};

TEST(DependencyGraph, FollowsInterfaceBindingToImplementationDependencies) {
    using RepositoryBinding =
        smol_di::Binding<^^IUserRepository, ^^UserRepository>;

    auto app = smol_di::create_container<UserController, RepositoryBinding>();

    auto &controller = app.get<UserController>();
    auto &repository = app.get<IUserRepository>();
    auto &config = app.get<RepositoryConfig>();

    EXPECT_EQ(&controller.service.repository, &repository);
    EXPECT_EQ(&repository.config(), &config);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
