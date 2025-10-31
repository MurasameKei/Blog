#ifndef AUTHENTICATION_MANAGEMENT
#define AUTHENTICATION_MANAGEMENT

#include <string>

enum privilege {
    Basic,
    EarlyAccess,
    Full,
    Moderator,
    Admin
};
bool canAccess(std::string sid, privilege priv);
int getuid(std::string sid);

#endif
