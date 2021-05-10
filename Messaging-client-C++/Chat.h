#pragma once
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <vector> 
#include<limits>
#include <sstream>
#include <boost/algorithm/hex.hpp>
#include <iomanip>
#include <intrin.h>
#include "connection.h"
#include "FilesHandler.h"
#include "ChatClient.h"
#include "OtherClient.h"
#include "AsymetricEncrypt.h"
#include "SymetricCrypt.h"
const int _4MB = 4000000;
const int WITH_SQL = 2, WITHOUT_SQL = 1;
const int MAX_NAME_LENGTH = 255;
const int ID_LENGTH = 16;
const int PUBLIC_KEY_LENGTH = 160,SYMETRIC_KEY_LENGTH=16;
const int REGISTER_PAYLOAD_SIZE = 287;
const int REGISTER_CODE = 100;
const int REGISTER_ANS = 1000;
const int CLIENTS_LIST_REQ = 101;
const int CLIENTS_LIST_ANS = 1001;
const int PUBLIC_KEY_REQ = 102;
const int PUBLIC_KEY_ANS = 1002;
const int WAITING_MESSAGES_REQ = 104;
const int WAITING_MESSAGES_ANS = 1004;
const int REQUEST_FOR_SKEY_TYPE = 1, SEND_SKEY_TYPE = 2, TEXT_MESS = 3;
const int SEND_MESS_REQ = 103,SEND_MESS_ANS=1003;
const std::string ME_INFO = ".\\me.info.txt",SERVER_INFO=".\\server.info.txt";

const int REQU_FAILED = 9000;

#pragma pack(push,1)
struct HeaderReq {
	uint8_t cid[ID_LENGTH];
	uint8_t vers;
	uint8_t code;
	uint32_t psize;
};
#pragma pack(pop)

#pragma pack(push,1)
struct HeaderAns {
	uint8_t vers;
	uint16_t code;
	uint32_t psize;

};
#pragma pack(pop)

#pragma pack(push,1)
struct RegisterPayl {
	uint8_t name[MAX_NAME_LENGTH];
	uint8_t public_key[PUBLIC_KEY_LENGTH];
};
#pragma pack(pop)


#pragma pack(push,1)
struct SendMessg {
	uint8_t cid[ID_LENGTH];
	uint8_t type;
	uint32_t cont_size;
};
#pragma pack(pop)

#pragma pack(push,1)
struct ClientsRecv {
	uint8_t cid[ID_LENGTH];
	uint8_t name[MAX_NAME_LENGTH];
};
#pragma pack(pop)

#pragma pack(push,1)
struct PublicKeyRecv {
	uint8_t cid[ID_LENGTH];
	uint8_t public_key[PUBLIC_KEY_LENGTH];
};
#pragma pack(pop)

#pragma pack(push,1)
struct MessgSent {
	uint8_t cid[ID_LENGTH];
	uint32_t messg_id;
};
#pragma pack(pop)

#pragma pack(push,1)
struct WaitingMessages {
	uint8_t cid[ID_LENGTH];
	uint32_t messg_id;
	uint8_t type;
	uint32_t cont_size;
};
#pragma pack(pop)

using  namespace std;
/*
 * this class represents the chat application-
 * connn- responsible for handling the connection with server
 * file_handler- responsible for handle files
 * client- represents this machine client
 * clients_list- saves the other machines clients
 * header_req- represents the header of requests
 * header_ans- represents the hader of replys
*/
class Chat {
	ConnHandler conn;
	FileHandler file_handler;
	ChatClient client;
	vector <OtherClient> clients_list;
	HeaderReq header_req;
	HeaderAns header_ans;
	bool displayMessages();
	void handleWaitingMessage(unsigned char* mess, size_t size, unsigned char* cid, uint8_t type);
	bool initializeClist();
	bool isRegistred();
	void reqForSkeyPacket(std::string id);
	void sendSkeyPacket(std::string id);
	string createSkey(std::string id);
	string getTextMessgFromUser();
	string getUserNameFromInput(string instruction);
	void sendTextMessPacket(std::string id);
	void SendMessageAns(std::string content,size_t type,std::string id);
	void WaitingMessagesAns();
	void publicKeyAns(std::string id);
	void ClientsListAns();
	bool handleAnsRecv(unsigned char* buff, size_t size);
	bool handleAnsRecv(vector<uint8_t> buff, size_t size);
	bool handleHeaderRecv(int code);
	void RegisterAns();
	std::string readPrivateKey();
	OtherClient* getClientByName(std::string name);
	OtherClient* getClientById(std::string id);
	public:
		Chat(tcp::socket& sock, tcp::resolver& resolver );
		ConnHandler getConnectionH();
		ChatClient getClient();
		void setHeaderId();
		void setHeaderReq(uint8_t code,uint32_t psize);
		virtual ~Chat();
		void RegisterReq();
		void ClientsListReq();
		void publicKeyReq();
		void WaitingMessagesReq();
		void SendMessageReq(int meesage_type);
		

	
};