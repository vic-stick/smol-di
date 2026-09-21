#include "smol_di/smol_di.hpp"

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

    void save() override {}
};

struct UserService {
    UserService(IUserRepository &repo, Database &db, Logger &logger) {}
};

struct UserController {
    UserController(UserService &service) {}
};

int main() {

    using RepositoryBinding =
        smol_di::Binding<^^IUserRepository, ^^UserRepository>;

    using Found =
        smol_di::find_binding<^^IUserRepository, RepositoryBinding>::type;

    static_assert(Found::implementation_info == ^^UserRepository);

    using DatabaseBinding =
        smol_di::find_binding<^^Database, RepositoryBinding>::type;

    static_assert(DatabaseBinding::implementation_info == ^^Database);
}
