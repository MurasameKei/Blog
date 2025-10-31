#include "server.h"
#include "session.h"
#include "crypto.h"
#include "b64.h"
#include "cpp-httplib/httplib.h"
#include <functional>
#include <iostream>
#include <string>
#include <nlohmann/json.hpp>
#include <openssl/rand.h>

#include <vector>
using nlohmann::json, std::vector, std::string, std::cout, std::endl;

struct SessionData {
    int lifeLeft = 32;
    int totalLife = 0;
    bool hasCrypto = false;
    int userid = -1;
};

std::map<string, SessionData> sessions;

string resolve(string sid) {
    if(sessions.count(sid) != 0) {
        sessions[sid].totalLife += 32 - sessions[sid].lifeLeft;
        sessions[sid].lifeLeft = 32;
        return "Life extended";
    } else {
        SessionData dat;
        dat.lifeLeft = 32;
        dat.totalLife = 0;
        sessions.insert(std::make_pair(sid, std::move(dat))); 
        return "New";
    }
}

int initSessionServer(httplib::Server* svr) {

    svr->Post("/api/session/resolve", [](const httplib::Request& req, httplib::Response& res) {
        res.set_content(encaps(resolve(req.body), "N/A"), "text/plain");
    });

    return 0;
}

vector<std::function<void(string)>> expireListeners;

void updateSessions() {
    for(std::map<string, SessionData>::iterator iter = sessions.begin(); iter != sessions.end();) {
        iter->second.lifeLeft -= 1;
        if(iter->second.lifeLeft <= 0) {
            for(int i = 0; i < expireListeners.size(); i++) {
                expireListeners[i](iter->first);
            }
            sessions.erase(iter++);
        }
        else { ++iter; }
    }
}

void addExpireListener(std::function<void(string)> listener) {
    expireListeners.push_back(listener);
}

void attachCrypto(string sid) {
    sessions[sid].hasCrypto = true;
}

void attachUserID(std::string sid, int uid) {
    sessions[sid].userid = uid;
}
