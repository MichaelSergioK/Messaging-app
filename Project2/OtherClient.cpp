#include "OtherClient.h"


/*
	constructor- name:the name of the client
				 id: the id of the client
*/
OtherClient::OtherClient(std::string name, std::string id) :name(name), id(id)
{
	got_sym_key = got_public_key = false;
	public_key[0] = 0;
	symetric_key[0] = 0;
}

std::string OtherClient::getName()
{
	return name;
}

unsigned char* OtherClient::getPublicKey()
{
	return  public_key;
}

unsigned char* OtherClient::getSymetricKey()
{
	return symetric_key;
}

std::string OtherClient::getId()
{
	return id;
}

void OtherClient::setName(std::string name)
{
	this->name = name;
}

void OtherClient::setPublicKey(unsigned char* public_key)
{
	memcpy_s(this->public_key,KEY_LENGTH,public_key,KEY_LENGTH);
	got_public_key = true;
}

void OtherClient::setSymetricKey(unsigned char* symetric_key)
{
	memcpy_s(this->symetric_key, SYM_KEY_LENGTH, symetric_key, SYM_KEY_LENGTH);
	got_sym_key = true;
}

void OtherClient::setId(std::string id)
{
	this->id = id;
}

//return true if the main client got this client public key, otherwise returns false
bool OtherClient::gotPubKey()
{
	return got_public_key;
}
//return true if the main client got this client symetric key, otherwise returns false
bool OtherClient::gotSymeKey()
{
	return got_sym_key;
}

std::ostream& operator<<(std::ostream& output, const OtherClient& other)
{
	output << "user name: " << other.name <<  std::endl;
	return output;
}




