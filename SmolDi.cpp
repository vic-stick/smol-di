#include "smol_di/smol_di.hpp"

#include <print>

struct Database {};

struct Logger {};

struct ServiceB;

struct ServiceA {
    ServiceA(ServiceB &);
};

struct ServiceB {
    ServiceB(ServiceA &);
};

struct IUserRepository {
    virtual ~IUserRepository() = default;
    virtual void save() = 0;
};

struct UserRepository : IUserRepository {
    UserRepository(Database &db) {}

    void save() override { std::println("UserRepository::save()"); }
};

struct UserService {
    IUserRepository &repository;

    UserService(IUserRepository &repo, Database &db, Logger &logger)
        : repository(repo) {}
};

struct UserController {
    UserController(UserService &service) {}
};

int main() {
    using RepositoryBinding =
        smol_di::Binding<^^IUserRepository, ^^UserRepository>;

    auto app = smol_di::create_container<UserController, RepositoryBinding>();

    auto &controller = app.get<UserController>();

    (void)controller;

    app.get<UserService>().repository.save();

    std::println("UserController constructed!");
}
