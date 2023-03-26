#include<crypto.h>


std::string sha256(const std::string& message) {
    unsigned char digest[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, message.c_str(), message.size());
    SHA256_Final(digest, &sha256);

    std::stringstream ss;
    for(int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(digest[i]);
    }

    return ss.str();
}

std::string aesEncrypt(std::string plaintext, const std::string& key, const std::string& iv) {
    std::string ciphertext;

    // Set up the AES key and IV
    AES_KEY aesKey;
    AES_set_encrypt_key((const unsigned char*)key.c_str(), key.size() * 8, &aesKey);

    // Pad the plaintext to a multiple of the block size
    size_t paddedLen = (plaintext.size() / AES_BLOCK_SIZE + 1) * AES_BLOCK_SIZE;
    std::string paddedPlaintext = plaintext;
    paddedPlaintext.resize(paddedLen, '\0');

    // Perform the AES encryption
    ciphertext.resize(paddedLen);
    AES_cbc_encrypt((const unsigned char*)paddedPlaintext.c_str(), (unsigned char*)ciphertext.data(),
                    paddedLen, &aesKey, (unsigned char*)iv.c_str(), AES_ENCRYPT);

    return ciphertext;
}

// Decrypt a string using AES
std::string aesDecrypt(std::string ciphertext, const std::string& key, const std::string& iv) {
    std::string plaintext;

    // Set up the AES key and IV
    AES_KEY aesKey;
    AES_set_decrypt_key((const unsigned char*)key.c_str(), key.size() * 8, &aesKey);

    // Perform the AES decryption
    plaintext.resize(ciphertext.size());
    AES_cbc_encrypt((const unsigned char*)ciphertext.c_str(), (unsigned char*)plaintext.data(),
                    ciphertext.size(), &aesKey, (unsigned char*)iv.c_str(), AES_DECRYPT);

    // Remove any padding added during encryption
    size_t paddingLen = plaintext[plaintext.size() - 1];
    plaintext.resize(plaintext.size() - paddingLen);

    return plaintext;
}

std::string md5(const std::string& input) {
    unsigned char digest[MD5_DIGEST_LENGTH];
    MD5((unsigned char*)input.c_str(), input.size(), digest);

    char md5String[33];
    for (int i = 0; i < 16; i++) {
        sprintf(&md5String[i*2], "%02x", (unsigned int)digest[i]);
    }
    md5String[32] = '\0';

    return std::string(md5String);
}

