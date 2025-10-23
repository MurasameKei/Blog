#ifndef POST_QUANTUM_CRYPTOGRAPHY_ENCAPSULATION
#define POST_QUANTUM_CRYPTOGRAPHY_ENCAPSULATION

#include <string>
#include <nlohmann/json.hpp>
namespace httplib{ class Server; }

int initCryptoServer(httplib::Server* server);
std::string encaps(std::string sendData, std::string SID, bool useEncryption = true);
std::pair<nlohmann::json, std::string> decaps(std::string rawData);

#endif
