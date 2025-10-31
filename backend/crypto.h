#ifndef POST_QUANTUM_CRYPTOGRAPHY_ENCAPSULATION
#define POST_QUANTUM_CRYPTOGRAPHY_ENCAPSULATION

#include <string>
#include <nlohmann/json.hpp>
std::string encaps(std::string sendData, std::string SID);
std::pair<nlohmann::json, std::string> decaps(std::string rawData);

#endif
