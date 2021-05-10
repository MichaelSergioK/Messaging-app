#pragma once
#include <rsa.h>
#include <osrng.h>
#include <base64.h>
#include <files.h>
#include <iostream>
#include <string>
#include <fstream>
#include <exception>      

const int KEY_L = 160;

/*
	This class contains static methos to handle asymetric encryption and decryption
*/
class  AsymetricEncrypt
{
public:
	static std::string generatePrivateKey();
	static std::string generatePublicKey(std::string  privkey);
	static void cutPubKey(std::string pubkey, unsigned char* buf);
	static std::string encryptMessage(CryptoPP::byte* buf, std::string message);
	static std::string decryptMessage(std::string privkey, std::string message);



private:

	static CryptoPP::RSAFunction pubKeyFromString(std::string key);

	static std::string pubicKeyToString(CryptoPP::RSAFunction& key);

	static CryptoPP::RSA::PrivateKey privateKeyFromString(std::string key);

	static std::string privateKeyToString(CryptoPP::RSA::PrivateKey& key);


};

