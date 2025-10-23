#include "b64.h"
#include <openssl/evp.h>
#include <iostream>
#include <stdexcept>
#include <vector>

char *base64(const unsigned char *input, int length) {
    const int pl = 4*((length+2)/3);
    auto output = reinterpret_cast<char *>(calloc(pl+1, 1)); //+1 for the terminating null that EVP_EncodeBlock adds on
    const int ol = EVP_EncodeBlock(reinterpret_cast<unsigned char *>(output), input, length);
    if (pl != ol) { std::cerr << "Whoops, encode predicted " << pl << " but we got " << ol << "\n"; }
    return output;
}

unsigned char *decode64(const char *input, int length) {
    const int pl = 3*length/4;
    unsigned char* output = reinterpret_cast<unsigned char *>(calloc(pl+1, 1));
    const int ol = EVP_DecodeBlock(output, reinterpret_cast<const unsigned char *>(input), length);
    if (pl != ol) {
        std::cerr << "Whoops, decode predicted " << pl << " but we got " << ol << "\n"; 
        throw std::runtime_error("Error: length of decoded block was different from predicted");
    }
    return output;
}

std::vector<unsigned char> B64ToVec(std::string input) {
    unsigned char* data = decode64(input.c_str(), input.length());    
    int predLen = 3*input.length()/4;
    if(data[predLen - 1] == '\0')
        predLen -= 1;
    if(data[predLen - 1] == '\0')
        predLen -= 1;
    if(predLen < 0)
        predLen = 0;
    std::vector<unsigned char> results(data, data+predLen) ;
    return std::move(results);
}

std::string VecToB64(std::vector<unsigned char> input) {
    return reinterpret_cast<char *>(base64(input.data(), input.size()));    
}

