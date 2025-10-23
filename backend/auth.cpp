#include "auth.h"
#include "session.h"
#include "crypto.h"
#include "cpp-httplib/httplib.h"
#include <nlohmann/json.hpp>
#include <string>

using nlohmann::json, std::vector, std::string, std::cout, std::endl;

struct AccountLogin {
    string username;
    string password;
    privilege priv = Basic;
};
vector<AccountLogin> avaliableAccounts;

struct SessionAccountData {
    string username;
    privilege priv;
};
std::map<string, SessionAccountData> sessionAccounts;

string login(json recieved, string sid) {
    for(int i = 0; i < avaliableAccounts.size(); i++) {
        if(recieved["username"] == avaliableAccounts[i].username) {
            if(recieved["password"] == avaliableAccounts[i].password) {
                SessionAccountData login;
                login.username = recieved["username"];
                login.priv = avaliableAccounts[i].priv;
                sessionAccounts.insert(std::make_pair(sid, std::move(login))); 
                return "Successfully loged in!";
            }
            else {
                return "Incorrect password!";
            }
        }
    }
    AccountLogin newAccount;
    newAccount.username = recieved["username"];
    newAccount.password = recieved["password"];
    if(newAccount.username == "EarlyAccess")
        newAccount.priv = EarlyAccess;
    if(newAccount.username == "Full")
        newAccount.priv = Full;
    if(newAccount.username == "Moderator")
        newAccount.priv = Moderator;
    if(newAccount.username == "Admin")
        newAccount.priv = Admin;
    avaliableAccounts.push_back(std::move(newAccount));
    return "Created new account!";
}

bool canAccess(std::string sid, privilege priv) {
    if(sessionAccounts.count(sid) == 0)
        return false;
    return sessionAccounts[sid].priv >= priv;
}

void expireAccount(string sid) {
    if(sessionAccounts.count(sid) == 0)
        return;
    sessionAccounts.erase(sid);
}

int initAuthServer(httplib::Server* svr) {

    svr->Post("/api/auth/login", [](const httplib::Request& req, httplib::Response& res) {
        std::pair<json, string> data = decaps(req.body);
        res.set_content(encaps(login(data.first, data.second), data.second), "text/plain");
    });

    addExpireListener(expireAccount);

    return 0;
}
