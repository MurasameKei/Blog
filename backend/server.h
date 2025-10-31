#ifndef SERVER_INIT_CALLS
#define SERVER_INIT_CALLS

namespace httplib{ class Server; }
int initCommentsServer(httplib::Server* svr);
int initAuthServer(httplib::Server* svr);
int initCryptoServer(httplib::Server* server);
int initSessionServer(httplib::Server* svr);

void updateSessions();

#endif
