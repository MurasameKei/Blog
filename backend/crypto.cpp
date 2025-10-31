#include "server.h"
#include "crypto.h"
#include "b64.h"
#include "session.h"
#include "cpp-httplib/httplib.h"
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <fstream>
#include <oqs/kem.h>
#include <oqs/kem_ml_kem.h>
#include <oqs/oqs.h>
#include <oqs/sig_ml_dsa.h>
#include <openssl/rand.h>
#include <openssl/evp.h>
#include <openssl/aes.h>
#include <openssl/conf.h>
#include <openssl/err.h>
#include <nlohmann/json.hpp>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

using nlohmann::json, std::vector, std::string, std::cout, std::endl;

vector<uint8_t> SignatureSecretKey;

struct SessionCrypto {
    OQS_KEM* kem;
    vector<uint8_t> publicKey;    
    vector<uint8_t> privateKey;
    vector<uint8_t> sharedKey;
    vector<uint8_t> clientPublicKey;    
};
std::map<string, SessionCrypto> sessionCrypto;

SessionCrypto generateSessionCrypto() {
    SessionCrypto crypto;
    crypto.kem = OQS_KEM_new(OQS_KEM_alg_ml_kem_768);
    if (crypto.kem == nullptr) {
        std::cerr << "Error generating key pair" << endl;
        exit(1);
    }
    vector<uint8_t> pubKey(crypto.kem->length_public_key);    
    vector<uint8_t> privKey(crypto.kem->length_secret_key);    
    crypto.kem->keypair(pubKey.data(), privKey.data());
    crypto.publicKey = std::move(pubKey);
    crypto.privateKey = std::move(privKey);
    return std::move(crypto);
}
std::pair<string, vector<uint8_t>> encrypt(string msg, vector<uint8_t> key) {
    EVP_CIPHER_CTX* ctx;
    ctx = EVP_CIPHER_CTX_new();

    vector<uint8_t> IV(16);
    RAND_bytes(IV.data(), 16);

    const int messageSize = msg.size() + (AES_BLOCK_SIZE - msg.size() % AES_BLOCK_SIZE);
    int len = 0;
    int ciphertext_len = 0;
    vector<uint8_t> ciphertext(messageSize);
    EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, key.data(), IV.data());
    EVP_EncryptUpdate(ctx, ciphertext.data(), &len, reinterpret_cast<const unsigned char*>(msg.data()), msg.size());
    ciphertext_len = len;
    EVP_EncryptFinal_ex(ctx, ciphertext.data()+len, &len);
    ciphertext_len += len;
    EVP_CIPHER_CTX_free(ctx);

    return std::make_pair(VecToB64(ciphertext), IV);
}
string decrypt(string input, vector<uint8_t> IV, vector<uint8_t> key) {
    vector<uint8_t> encrypted = B64ToVec(input);

    EVP_CIPHER_CTX* ctx;
    ctx = EVP_CIPHER_CTX_new();

    int len = 0;
    int plaintext_len = 0;
    vector<uint8_t> plaintext(encrypted.size());
    EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, key.data(), IV.data());
    //unsigned char* ciphertext = new unsigned char[encrypted.length()];
    EVP_DecryptUpdate(ctx, plaintext.data(), &len, encrypted.data(), encrypted.size());
    plaintext_len = len;
    EVP_DecryptFinal_ex(ctx, plaintext.data()+len, &len);
    plaintext_len += len;

    string data(reinterpret_cast<char*>(plaintext.data()), plaintext_len);
    return data;
}

string encaps(string sendData, string SID) {
    string payload = sendData;
    string iv = "N/A";
    if (SID != "N/A"){
        std::pair<string, vector<uint8_t>> encrypted = encrypt(sendData, sessionCrypto[SID].sharedKey);
        payload = encrypted.first;
        iv = VecToB64(encrypted.second);
    }
    vector<uint8_t> dataBytes(payload.c_str(), payload.c_str() + payload.length());

    std::vector<uint8_t> sig(OQS_SIG_ml_dsa_44_length_signature);
    size_t signature_len;

    if(OQS_SIG_ml_dsa_44_sign(sig.data(),
            &signature_len,
            dataBytes.data(),
            dataBytes.size(),
            SignatureSecretKey.data()) != 0)
    {
        throw std::runtime_error("Security risk: failed to sign the payload!");
    }
    json output;
    output["Payload"] = payload;
    output["IV"] = iv;
    output["Signature"] = VecToB64(sig);
    return output.dump();
}
std::pair<json, string> decaps(string rawData) {
    json recieved = json::parse(rawData);
    string SID = recieved["SessionID"];
    string payloadData = recieved["Payload"];
    vector<uint8_t> dataBytes(payloadData.c_str(), payloadData.c_str() + payloadData.length());
    vector<uint8_t> signature = B64ToVec(recieved["Signature"]);

    if(OQS_SIG_ml_dsa_44_verify(dataBytes.data(),
            payloadData.size(),
            signature.data(),
            signature.size(),
            sessionCrypto[SID].clientPublicKey.data()) != 0)
    {
        throw std::runtime_error("Security risk: Signiture did not match! Modified durring transit!");
        return std::make_pair("", "");
    }

    if(recieved["IV"] != "N/A") {
        payloadData = decrypt(payloadData, B64ToVec(recieved["IV"]), sessionCrypto[SID].sharedKey);
    }

    json payload = json::parse(payloadData);
    auto retval = std::make_pair(payload, SID);
    return retval;
}

string handshake(string rawData) {
    // Get client public key
    json recieved = json::parse(rawData);
    vector<uint8_t> clientPubKey = recieved["PubKey"].get<vector<uint8_t>>();

    SessionCrypto crypto = generateSessionCrypto();
    string retval = VecToB64(crypto.publicKey);
    crypto.clientPublicKey = std::move(clientPubKey);

    // Save session
    sessionCrypto.insert( std::pair<string, SessionCrypto>
            (recieved["SessionID"], std::move(crypto)) );

    // Send Public key 
    return retval;
}

string completeHandshake(json recieved, string sid) {
    vector<uint8_t> ciphertext = recieved["cipherText"].get<vector<uint8_t>>();

    vector<uint8_t> sharedSecret(sessionCrypto[sid].kem->length_shared_secret);    
    sessionCrypto[sid].kem->decaps(sharedSecret.data(), ciphertext.data(), sessionCrypto[sid].privateKey.data());

    recieved["res"] = sharedSecret;
    sessionCrypto[sid].sharedKey = std::move(sharedSecret);
    return "The handshake is done successfully!";
}

void generateKeypair() {
    vector<uint8_t> SigPub(OQS_SIG_ml_dsa_44_length_public_key);
    vector<uint8_t> SigPriv(OQS_SIG_ml_dsa_44_length_secret_key);
    OQS_SIG_ml_dsa_44_keypair(SigPub.data(), SigPriv.data());
    cout << "Public Key: " << endl << VecToB64(SigPub)<< endl;
    std::ofstream outFile("SignatureSecretKey", std::ios::binary);
    if (!outFile)
        throw std::runtime_error("Failed to create keyfile");
    outFile.write(reinterpret_cast<const char*>(SigPriv.data()), SigPriv.size());
    outFile.close();
}

void expireCrypto(string sid) {
    sessionCrypto.erase(sid);
}

int initCryptoServer(httplib::Server* svr) {
    std::ifstream inFile("SignatureSecretKey", std::ios::binary);
    if (!inFile)
        throw std::runtime_error("Failed to read secret key");
    vector<uint8_t> SigPriv(OQS_SIG_ml_dsa_44_length_secret_key);
    inFile.read(reinterpret_cast<char*>(SigPriv.data()), OQS_SIG_ml_dsa_44_length_secret_key);
    inFile.close();
    SignatureSecretKey = std::move(SigPriv);


    svr->Post("/api/auth/handshake", [](const httplib::Request& req, httplib::Response& res) {
        res.set_content(encaps(handshake(req.body), "N/A"), "text/plain");
    });

    svr->Post("/api/auth/complete_handshake", [](const httplib::Request& req, httplib::Response& res) {
        std::pair<json, string> data = decaps(req.body);
        string send = encaps(completeHandshake(data.first, data.second), data.second);
        res.set_content(send, "text/plain");
    });
    
    addExpireListener(expireCrypto);
    
    return 0;
}
