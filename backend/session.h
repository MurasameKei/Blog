#ifndef SESSION_MANAGEMENT
#define SESSION_MANAGEMENT

#include <functional>
#include <string>

void startSession(std::string sid);
void attachCrypto(std::string sid);
void attachUserID(std::string sid, int uid);

void addExpireListener(std::function<void(std::string)> listener);

#endif
