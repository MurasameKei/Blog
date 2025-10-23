#ifndef BASE_64_STRING_UTILS
#define BASE_64_STRING_UTILS

#include <string>
#include <vector>
std::vector<unsigned char> B64ToVec(std::string input);
std::string VecToB64(std::vector<unsigned char> input);

#endif
