#pragma once
#include <openssl/sha.h>
#include <openssl/aes.h>
#include <openssl/md5.h>
#include <string.h>
#include <iostream>
#include <iomanip>
#include <sstream>

std::string sha256(const std::string& message); 
std::string md5(const std::string& input);
std::string aesEncrypt(std::string plaintext, const std::string& key, const std::string& iv);
std::string aesDecrypt(std::string ciphertext, const std::string& key, const std::string& iv);
/* std::string aesEncrypt(const std::string& password, const std::string& data) */
/* std::string aesDecrypt(const std::string& password, const std::string& strData) */
