#ifndef AUTHENTICATION_MANAGEMENT
#define AUTHENTICATION_MANAGEMENT

#include <string>
namespace httplib{ class Server; }
int initAuthServer(httplib::Server* svr);

enum privilege {
    Basic,
    EarlyAccess,
    Full,
    Moderator,
    Admin
};
bool canAccess(std::string sid, privilege priv);

#endif
