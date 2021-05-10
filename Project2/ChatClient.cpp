#include"ChatClient.h"


ChatClient::ChatClient():user_name(""),userID(""),public_key(""),symetric_key(""),private_key("") {
}
std::string ChatClient::getUserName() {
	return user_name;
}
std::string ChatClient::getUserID() {
	return userID;
}
std::string ChatClient::getSymetricKey() {
	return symetric_key;
}
std::string ChatClient::getPublicKey()
{
	return public_key;
}
std::string ChatClient::getPrivateKey()
{
	return private_key;
}
void ChatClient::setUserName(std::string name) {
		user_name = name;
}
void ChatClient::setUserID(std::string id) {
		userID = id;
}
void ChatClient::setSymetricKey(std::string key) {
		symetric_key = key;
}

void ChatClient::setPublicKey(std::string key)
{
		public_key = key;
}
void ChatClient::setPrivateKey(std::string key)
{
		private_key = key;
}
