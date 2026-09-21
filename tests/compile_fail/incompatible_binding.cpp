#include "smol_di/smol_di.hpp"

struct IUserRepository {
    virtual ~IUserRepository() = default;
};

struct UserService {
    explicit UserService(IUserRepository &) {}
};

struct Database {};

int main() {
    auto app = smol_di::create_container<UserService>(
        smol_di::bind<IUserRepository, Database>);
    (void)app;
}
