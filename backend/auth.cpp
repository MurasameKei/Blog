#include "server.h"
#include "auth.h"
#include "session.h"
#include "crypto.h"
#include "cpp-httplib/httplib.h"
#include <cstdint>
#include <nlohmann/json.hpp>
#include <random>
#include <string>
#include <pqxx/pqxx>

using nlohmann::json, std::vector, std::string, std::cout, std::endl;

struct SessionAccountData {
    int uid;
    string username;
    int16_t priv;
};
std::map<string, SessionAccountData> sessionAccounts;

string login(json recieved, string sid) {
    pqxx::connection C("dbname=Blog user=postgres host=/run/postgresql");
    if (!C.is_open())
        throw std::runtime_error("Failed to open database");
    pqxx::nontransaction N(C);

    string sql = "SELECT uid, password, privilege, pfpURL FROM users WHERE username = $1 "; 
    string username = recieved["username"].get<string>();
    string password = recieved["password"].get<string>();
    pqxx::params params = {username};
    pqxx::result R(N.exec(sql, params));
    C.close();

    json returning;
    if(R.size() == 1) {
        if(password == R.begin()[1].as<string>()) {
            SessionAccountData login;
            login.username = username;
            login.priv = R.begin()[2].as<int16_t>();
            login.uid = R.begin()[0].as<int>();
            sessionAccounts.insert(std::make_pair(sid, std::move(login))); 
            returning["status"] = "Success!";
            returning["username"] = username;
            returning["priv"] = R.begin()[2].as<int16_t>();
            returning["uid"] = R.begin()[0].as<int>();
            returning["pfpURL"] = R.begin()[3].as<string>();
        }
        else {
            returning["status"] = "Incorrect password.";
        }
    }
    else if (R.size() != 0){
        throw std::runtime_error("Multiple accounts of the same username exist");
    }
    else {
        returning["status"] = "Account does not exits";
    }
    return returning.dump();
}

string signUp(json recieved, string sid) {
    json returning;
    pqxx::connection C("dbname=Blog user=postgres host=/run/postgresql");
    if (!C.is_open())
        throw std::runtime_error("Failed to open database");
    pqxx::nontransaction N(C);

    string username = recieved["username"].get<string>();
    string password = recieved["password"].get<string>();

    string sql = "SELECT EXISTS (SELECT 1 FROM users WHERE username = $1)"; 
    pqxx::params params = {username};

    bool accountExists = N.exec(sql, params)[0][0].as<bool>();
    if (accountExists) {
        returning["status"] = "Account already exists";
        return returning.dump();
    }
    N.abort();

    pqxx::work W(C);
    int16_t priv = Basic;
    if(username == "EarlyAccess")
        priv = EarlyAccess;
    if(username == "Full")
        priv = Full;
    if(username == "Moderator")
        priv = Moderator;
    if(username == "Admin")
        priv = Admin;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(0, 82);

    string pfpURL = "/res/pfp/" + std::to_string(distr(gen)) + ".png";
    cout << pfpURL << endl;

    sql = "INSERT INTO USERS (username, password, pfpURL, privilege) VALUES ($1, $2, $3, $4) RETURNING uid";
    params = {username, password, pfpURL, priv};

    int uid = W.exec(sql, params)[0][0].as<int>();
    W.commit();
    C.close();

    SessionAccountData login;
    login.username = username;
    login.priv = priv;
    login.uid = uid;
    sessionAccounts.insert(std::make_pair(sid, std::move(login))); 

    returning["status"] = "Success!";
    returning["username"] = username;
    returning["priv"] = priv;
    returning["uid"] = uid;
    returning["pfpURL"] = pfpURL;

    return returning.dump();
}

string logout(std::string sid) {
    if(sessionAccounts.count(sid) == 0)
        return "No account to log out from";
    sessionAccounts.erase(sid);
    return "Logged out successfully.";
}

bool canAccess(std::string sid, privilege priv) {
    if(sessionAccounts.count(sid) == 0)
        return false;
    return sessionAccounts[sid].priv >= priv;
}

int getuid(std::string sid) {
    if(sessionAccounts.count(sid) == 0)
        return -1;
    return sessionAccounts[sid].uid;
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

    svr->Post("/api/auth/signup", [](const httplib::Request& req, httplib::Response& res) {
        std::pair<json, string> data = decaps(req.body);
        res.set_content(encaps(signUp(data.first, data.second), data.second), "text/plain");
    });

    svr->Post("/api/auth/logout", [](const httplib::Request& req, httplib::Response& res) {
        std::pair<json, string> data = decaps(req.body);
        res.set_content(encaps(logout(data.second), data.second), "text/plain");
    });

    addExpireListener(expireAccount);

    return 0;
}
