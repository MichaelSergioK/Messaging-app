#pragma once
#include <modes.h>
#include <aes.h>
#include <filters.h>
#include <exception>
#include <iostream>
#include <string>
#include <immintrin.h>	// _rdrand32_step

/*
This class contains static methods for Symetric encryption and decryption
*/
class SymetricCrypt {
	// size must be a 4 bytes align.
	public:
		static char* generate_key(char* buff, size_t size);
		static std::string encryptMessage(std::string message,unsigned char* key);
		static std::string decryptMessage(std::string message, unsigned char* key);
};