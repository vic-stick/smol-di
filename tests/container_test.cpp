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
    auto app = smol_di::create_container<UserService>(
        smol_di::bind<IUserRepository, UserRepository>);

    auto &service = app.get<UserService>();
    auto &repository = app.get<IUserRepository>();

    EXPECT_EQ(&service.repository, &repository);
}
