#include "cpp-httplib/httplib.h"
#include <thread>
#include "server.h"

void Update() {
    while(true) {
        sleep(15);
        updateSessions();
    }
}

int main() {
    httplib::Server svr;

    svr.Get("/api/ping", [](const httplib::Request& req, httplib::Response& res) {
        res.set_content("Ping!", "text/plain");
    });

    initSessionServer(&svr);
    initCryptoServer(&svr);
    initAuthServer(&svr);
    initCommentsServer(&svr);

    std::thread update(Update);

    svr.listen("localhost", 8080);
    update.join();
}
