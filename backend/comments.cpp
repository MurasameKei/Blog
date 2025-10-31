#include "server.h"
#include "cpp-httplib/httplib.h"
#include "auth.h"
#include "crypto.h"
#include <stdexcept>
#include <string>
#include <pqxx/pqxx>
#include <nlohmann/json.hpp>

using nlohmann::json, std::vector, std::string, std::cout, std::endl;

std::string getComments() {
    pqxx::connection C("dbname=Blog user=postgres host=/run/postgresql");
    if (!C.is_open())
        throw std::runtime_error("Failed to open database");
    string sql = "SELECT payload, timestamp, username, pfpURL FROM comments, users WHERE comments.uid = users.uid"; 
    pqxx::nontransaction N(C);
    pqxx::result R(N.exec(sql));
    json commentJson;
    int i = 0;
    for (pqxx::result::const_iterator c = R.begin(); c != R.end(); ++c) {
        commentJson[i]["payload"] = c[0].as<string>();
        commentJson[i]["timestamp"] = c[1].as<string>();
        commentJson[i]["username"] = c[2].as<string>();
        commentJson[i]["pfpURL"] = c[3].as<string>();
        i++;
    }
    C.close();
    return commentJson.dump();
}

string postComment(json recieved, string sid) {
    int uid = getuid(sid);
    if(uid == -1){
        throw std::runtime_error("Can not post comment without an account!");
    }
    pqxx::connection C("dbname=Blog user=postgres host=/run/postgresql");
    if (!C.is_open())
        throw std::runtime_error("Failed to open database");
    pqxx::work W(C);
    string sql = "INSERT INTO Comments (pageid, uid, payload, timestamp) VALUES ($1, $2, $3, $4)";

    int pageid = 1;
    string payload = recieved["payload"].get<string>();
    string timestamp = recieved["timestamp"].get<string>();
    pqxx::params params = {pageid, uid, payload, timestamp};

    W.exec(sql, params);
    W.commit();
    C.close();

    return "Complete!";
}

int initCommentsServer(httplib::Server* svr) {

    svr->Get("/api/load_comments", [](const httplib::Request& req, httplib::Response& res) {
        res.set_content(encaps(getComments(), "N/A"), "text/plain");
    });

    svr->Post("/api/post_comment", [](const httplib::Request& req, httplib::Response& res) {
        std::pair<json, string> data = decaps(req.body);
        res.set_content(encaps(postComment(data.first, data.second), data.second), "text/plain");
    });

    return 0;
}
