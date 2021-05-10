#pragma once
#include <string>

/*
This class represents this machine client by is- name, id, symetric key, public key and private key
*/
class ChatClient {

	std::string user_name;
	std::string userID;
	std::string symetric_key;
	std::string public_key;
	std::string private_key;
	public:
	ChatClient();
	std::string getUserName();
	std::string getUserID();
	std::string getSymetricKey();	
	std::string getPublicKey();

	std::string getPrivateKey();

	void setUserName(std::string name);
	void setUserID(std::string id);
	void setSymetricKey(std::string key);
	void setPublicKey(std::string key);

	void setPrivateKey(std::string key);

};