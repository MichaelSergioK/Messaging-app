#pragma once

#include <iostream>
#include <ostream>
#include <string>
#include <sstream>
#include <iomanip>

const int KEY_LENGTH = 160, SYM_KEY_LENGTH=16;

/*
	this class represents a user in the chat application
	for every user we save his id,username, public key and symetric key
*/
class OtherClient {
	bool got_public_key;
	bool got_sym_key;
	std::string name;
	std::string id;
	unsigned char  public_key [KEY_LENGTH];
	unsigned char symetric_key[SYM_KEY_LENGTH];

	public:
		OtherClient(std::string name, std::string id);
		std::string getName();
		unsigned char* getPublicKey();
		unsigned char* getSymetricKey();
		std::string getId();
		void setName(std::string name);
		void setPublicKey(unsigned char* public_key);
		void setSymetricKey(unsigned char* symetric_key);
		void setId(std::string id);
		bool gotPubKey();
		bool gotSymeKey();
		friend std::ostream& operator<<(std::ostream& output, const OtherClient &other);
	




};