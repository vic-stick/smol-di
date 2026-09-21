#include "smol_di/smol_di.hpp"

struct IUserRepository {
    virtual ~IUserRepository() = default;
};

struct UserService {
    explicit UserService(IUserRepository &) {}
};

struct Database {};

using BadBinding = smol_di::Binding<^^IUserRepository, ^^Database>;

int main() {
    auto app = smol_di::create_container<UserService, BadBinding>();
    (void)app;
}
