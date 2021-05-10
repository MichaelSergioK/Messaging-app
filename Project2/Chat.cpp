#include "Chat.h"
//checks if the system numeric values byte are in litle endian order
static bool isLitleE() {
	int n = 1;
	// little endian if true
	if (*(char*)&n == 1) return true;
	return false;
}

//initialize a string with an unsigned chars buffer
static string str_initizalization(unsigned char* buff, size_t size)
{
	std::string str;
	for (size_t i = 0; i < size; i++)
		str.push_back(buff[i]);
	return str;
}

//thransforms a byte representation to hexadecimal representation
static string hexStr(unsigned char* data, int len)
{
	std::stringstream ss;
	ss << std::hex;
	for (int i(0); i < len; ++i)
		ss << std::setw(2) << std::setfill('0') << (int)data[i];
	return ss.str();
}


/*
* constructs the Chat object-
* reads the details of the server
* initialize this client details if is registred
* 
*/
Chat::Chat(tcp::socket& sock, tcp::resolver& resolver):conn(sock,resolver), file_handler(SERVER_INFO){
	if (!file_handler.open()) {
		cout << "error- server not found" << endl;
		exit(1);
	}
	string line = file_handler.readLine();
	string ip = "";
	string port = "";
	size_t i = 0;
	for (;i<line.size() && line[i] != ':'; i++)
		ip.push_back(line[i]);
	i++;
	for(;i<line.size();i++)
		port.push_back(line[i]);
	conn.setIp(ip);
	conn.setPort(port);	
	header_req.vers = WITHOUT_SQL;
	header_ans.vers = WITHOUT_SQL;
	file_handler.close();
	if (isRegistred())
	{
		file_handler.open();
		string name = file_handler.readLine();
		string id = file_handler.readLine();
		string private_key = readPrivateKey();
		client.setUserID(boost::algorithm::unhex(id));
		memcpy_s(header_req.cid,ID_LENGTH,client.getUserID().c_str(),ID_LENGTH);
		client.setUserName(name);
		client.setPrivateKey(private_key);
		file_handler.close();
	}	
}

//read the prviate key from the file
std::string Chat::readPrivateKey()
{
	string private_key = "";
	string line = file_handler.readLine();
	while (line != "") {
		private_key += line;
		line = file_handler.readLine();

	}
	return private_key;

}

ConnHandler Chat::getConnectionH() {
	return conn;
}

ChatClient Chat::getClient() {
	return client;
}

void Chat::setHeaderId()
{
	memcpy_s((char*)header_req.cid,ID_LENGTH, client.getUserID().c_str(), ID_LENGTH);
}
//initialize the header request struct
void Chat::setHeaderReq(uint8_t code, uint32_t psize)
{
	header_req.code = code;
	header_req.psize = psize;
	if (!isLitleE())
		header_req.psize = _byteswap_ulong(psize);
}


 Chat::~Chat() {
	delete(&conn);
	delete(&file_handler);
 }
 //returns true if this client is already registred, otherwise false.
 bool Chat::isRegistred() {
	 file_handler.setFileName(ME_INFO);
	 if (!file_handler.exist(file_handler.getFileName()))
	 {
		 return false;
	 }
	 return true;
 }
 //recive the header reply from the server, return true in case of success in connection and if the request wa handled succesly
 bool Chat::handleHeaderRecv(int code) {
	 bool succeed = true;
	 if (!conn.recvRequest((unsigned char*)&header_ans, sizeof(header_ans)))
		 succeed = false;
	 if (header_ans.code != code)
	 {
		 cout << "error - server failed to handle your request" << endl;
		 succeed = false;
	 }
	 if (succeed == false) {
		 file_handler.close();
		 conn.disconnect();
	 }
	 return succeed;
 }

 //recive the payload reply from the server to an uchar buff
 // return true in case of success in connection and if the request wa handled succesly
 bool Chat::handleAnsRecv(unsigned char* buff, size_t size) {
	 if (!conn.recvRequest(buff, size))
	 {
		 cout << "Failed to recive the reply from the server" << endl;
		 conn.disconnect();
		 file_handler.close();
		 return false;
	 }
	 return true;
 }



 //recive the payload reply from the server to a vector buff
 // return true in case of success in connection and if the request wa handled succesly
 bool Chat::handleAnsRecv(vector <uint8_t> buff, size_t size) {
	 if (!conn.recvRequest(buff, size))
	 {
		 cout << "Failed to recive the reply from the server" << endl;
		 conn.disconnect();
		 file_handler.close();
		 return false;
	 }
	 return true;
 }

 /*
 handles the register request
 */
 void Chat::RegisterReq() {
	 file_handler.setFileName(ME_INFO);
	 if (file_handler.exist(file_handler.getFileName()))
	 {
		 cout << "you are already registred!" << endl;
		 return;
	 }
	 setHeaderReq(REGISTER_CODE, MAX_NAME_LENGTH + PUBLIC_KEY_LENGTH);//initialize the header of request
	 string name=getUserNameFromInput("enter your user name:");
	 if (name.size() > MAX_NAME_LENGTH-1)
	 {
		 cout << "error: the maximum length of username is 254 characters! try again."<<endl;//1 character is saved for the null char
			 return;
	 }
	 RegisterPayl payload;
	 memcpy_s(payload.name,name.size()+1,name.c_str(),name.size()+1);//+1 for the null terminated char
	 //need to create a public key
	 string private_key = AsymetricEncrypt::generatePrivateKey();
	 string public_key = AsymetricEncrypt::generatePublicKey(private_key);
	 unsigned char cutted_pub_key[PUBLIC_KEY_LENGTH];//Note on key size: We use 160 character here becuase our key is 1024 bits in size (the actual public key contains more data then just "the key")
	 AsymetricEncrypt::cutPubKey(public_key, cutted_pub_key);
	 if (private_key.size() == 0 || public_key.size() == 0)//failed to generate a key
		 return;
	 memcpy_s(payload.public_key,PUBLIC_KEY_LENGTH, cutted_pub_key,PUBLIC_KEY_LENGTH);
	 client.setPrivateKey(private_key);
	 client.setPublicKey(public_key);
	 client.setUserName(name);
	 if (!conn.connect())
		 return;
	 cout << "waiting for reply" << endl;
	 if (!conn.sendRequest((unsigned char*)&header_req, sizeof(header_req))) return;
	 if(!conn.sendRequest((unsigned char*)&payload,sizeof(payload))) return;
	 
	 RegisterAns();

 }

 /*
 * handles the reply from the server-
 * recive the id for the client and save it.
 */
 void Chat::RegisterAns() {
	 unsigned char buff[ID_LENGTH];
	 if (!handleHeaderRecv(REGISTER_ANS))
		 return;
	 if (!handleAnsRecv(buff,ID_LENGTH))
		 return;
	 string hexId = hexStr(buff, ID_LENGTH);
	 string id=str_initizalization(buff,ID_LENGTH);
	 file_handler.createFile(ME_INFO);
	 file_handler.writeLine(client.getUserName()+"\n"+hexId+"\n"+client.getPrivateKey());//write this client details to me.info
	 client.setUserID(id);
	 memcpy_s(header_req.cid, ID_LENGTH, id.c_str(), ID_LENGTH);
	 cout << "register completed, your id is: " << hexId << endl;
	 conn.disconnect();
	 file_handler.close();
 }



 
 /*
 * handles the request for clients list
 */
 void Chat::ClientsListReq() {
	 if (!isRegistred())//if this client has not registred
	 { 
		 cout << "Please register first!" << endl;
		 return;
	 }
	 header_req.code = CLIENTS_LIST_REQ;
	 header_req.psize = 0;
	 if (!conn.connect())
		 return;
	 if (!conn.sendRequest((unsigned char*)&header_req, sizeof(header_req))) return;
	 ClientsListAns();
 }

 /*handles the reply to the client list request-
 * recive all the clients and saves them to the client list
 */

 void Chat::ClientsListAns()
 {
	 if (!handleHeaderRecv(CLIENTS_LIST_ANS))
		 return;
	 if(!initializeClist())
		 return;
	 for (size_t i = 0; i < clients_list.size(); i++) //prints the clients names
		 cout << clients_list[i];
	 file_handler.close();
	 conn.disconnect();
 }

 /*
 * intialize the clients list- recive from server all the clients.
 -returns true in case of success, otherwise- false
 */
 bool Chat::initializeClist() {
	 ClientsRecv recv;
	 for (size_t i = 0; i < header_ans.psize / (MAX_NAME_LENGTH + ID_LENGTH); i++) {
		 if (!handleAnsRecv((unsigned char*)&recv, (MAX_NAME_LENGTH + ID_LENGTH)))
			 return false;
		 if (i == 0)
			 clients_list.clear();
		 string id=str_initizalization(recv.cid,ID_LENGTH);
		 string name = "";
		 for (int j = 0; recv.name[j] != '\0' && j < MAX_NAME_LENGTH-1; j++)//extracts the name from the 255 chars buffer
				name += recv.name[j];
		 OtherClient temp(name, id);
		 clients_list.push_back(temp);
	}
	 return true;
 }
 
 /*
 * handles the request for public key
 */
 void Chat::publicKeyReq() {
	 if (!isRegistred())
	 {
		 cout << "Please register first!" << endl;
		 return;
	 }
	 setHeaderReq(PUBLIC_KEY_REQ,ID_LENGTH);
	 string clientRequested = getUserNameFromInput("Enter the name of the client:");
	 OtherClient* temp = getClientByName(clientRequested);
	 if (temp==nullptr)
	 {
		 cout << "This client not exist" << endl;
		 return;
	 }
	 if (!conn.connect())
		 return;
	 unsigned char id[ID_LENGTH];
	 memcpy_s(id,ID_LENGTH,(*temp).getId().c_str(),ID_LENGTH);
	 if(!conn.sendRequest((unsigned char*)&header_req, sizeof(header_req))) return;
	 if(!conn.sendRequest(id, ID_LENGTH)) return;
	 publicKeyAns(clientRequested);
}
 
 /*
 * handles the reply for the public key request.
 * in case of success- save the public key for the user
 */
 void Chat::publicKeyAns(string name) {
	 PublicKeyRecv recv;
	 if (!handleHeaderRecv(PUBLIC_KEY_ANS))
		 return;
	 if (!handleAnsRecv((unsigned char*)&recv, (PUBLIC_KEY_LENGTH + ID_LENGTH)))
		 return;
	 getClientByName(name)->setPublicKey(recv.public_key);
	 cout << "Public key recived." << endl;
	 file_handler.close();
	 conn.disconnect();
 }

 /*
 * handle the request for waiting messages
 */
 void Chat:: WaitingMessagesReq() {
	 if (!isRegistred())
	 {
		 cout << "Please register first!" << endl;
		 return;
	 }
	 setHeaderReq(WAITING_MESSAGES_REQ, 0);
	 if (!conn.connect()) return;
	 if (!conn.sendRequest((unsigned char*)&header_req, sizeof(header_req))) return;
	 WaitingMessagesAns();
 }

 /*
 * handles the reply for the waiting messages request/
 */
 void Chat::WaitingMessagesAns() {
	 if (!handleHeaderRecv(WAITING_MESSAGES_ANS))
		 return;
	 if (!displayMessages()) {
		 cout << "error: couldnt see all the messages thats waiting for you" << endl;
		 return;
	 }
 }

 /*
 * recive the waiting messages and displays them
 */
 bool Chat::displayMessages() {
	 WaitingMessages recv;
	 unsigned int content_size=0;
	 unsigned char* content;
	 for (size_t i = header_ans.psize; i > 0; i -= (content_size + sizeof(WaitingMessages)))
	 {
		 if (!handleAnsRecv((unsigned char*)&recv, sizeof(recv)))
			 return false;
		 content_size = recv.cont_size;
		 content = new unsigned char [content_size];
		 if (!handleAnsRecv(content, content_size))
		 {
			 return false;
			 delete [] content;
		 }
		 handleWaitingMessage(content,content_size, (unsigned char*)recv.cid,recv.type);
		 delete [] content;
	 }
	 return true;
 }
 /*
 * handle a recived message-
 * categorize the message by is type-
 * if its  a request for symetric key- displays the request
 * if its a symetric key - decrypts the key and save the key for the sending user
 * if its a text message- decrypts the message(if got the symetric key) and display it 
 */
 void Chat::handleWaitingMessage(unsigned char* mess, size_t size,unsigned char* cid,uint8_t type) {
	
	 string message = str_initizalization(mess, size);//save the message to a string
	 string clid = str_initizalization(cid,ID_LENGTH);
	 OtherClient *temp = getClientById(clid);
	 if (temp != nullptr) {//display only the messages of clients thats wthis client have in the clist
		 cout << "From: " << temp->getName() << ":\ncontent:" << endl;
		 //decrypt message
		 switch (type) {
		 case 1: cout << "Request for symetric key" <<"\n----<EOM>----"<< endl; break;
		 case 2: cout << "Symetric key recived" <<"\n----<EOM>----" <<endl;
			 {
			   string decrypted_key = AsymetricEncrypt::decryptMessage(client.getPrivateKey(), message);
			   if (decrypted_key.size() != 0) // decryption succeed
				   temp->setSymetricKey((unsigned char*)decrypted_key.c_str());
			 }
			 break;
		 case 3:
			 if (temp->gotSymeKey()) {//if have the sym key for the sending client
				 string decrypted = SymetricCrypt::decryptMessage(message, temp->getSymetricKey());
				 if (decrypted.size()!=0)//decrypt succeed
					 cout << decrypted <<"\n----<EOM>----" <<endl; //print the decrypted message
			 }
			else//dont have the symetric key
					cout << "cannot decrypt the message, symetric key is missing."<< "\n----<EOM>----" <<endl;
			break; 
				
		 default:cout << "unexpected messag" << "\n----<EOM>----" << endl;
			     break;
		 }
	 }
 }

 /*
 * handles send message request-
 */
 void Chat::SendMessageReq(int meesage_type) {
	 if (!isRegistred())
	 {
		 cout << "Please register first!" << endl;
		 return;
	 }
	 string name = getUserNameFromInput("enter the destination user name:");
	 if (getClientByName(name)==nullptr)
	 {
		 cout << "cannot rcognize this client. try again!" << endl;
		 return;
	 }
	 switch (meesage_type) {
		 case REQUEST_FOR_SKEY_TYPE:reqForSkeyPacket(getClientByName(name)->getId()); break;
		 case SEND_SKEY_TYPE: sendSkeyPacket(getClientByName(name)->getId()); break;
		 case TEXT_MESS: sendTextMessPacket(getClientByName(name)->getId()); break;
		 default: break;
	 }
 }

 /*
 * handles end message request- request for symetric key
 */
 void Chat::reqForSkeyPacket(string id) {
	 SendMessg payload;
	 setHeaderReq(SEND_MESS_REQ, sizeof(SendMessg));
	 memcpy_s(payload.cid, ID_LENGTH, id.c_str(), ID_LENGTH);
	 payload.type = REQUEST_FOR_SKEY_TYPE;
	 payload.cont_size = 0;
	 if (!conn.connect()) return;
	 if (!conn.sendRequest((unsigned char*)&header_req, sizeof(header_req))) return;
	 if (!conn.sendRequest((unsigned char*)&payload, sizeof(SendMessg))) return;
	 SendMessageAns("", REQUEST_FOR_SKEY_TYPE,id);

 }
 /*
 * handles sending symetric key-
 * create symetric key if needed, encrypt the symetric key and send it
 */
 void Chat::sendSkeyPacket(string id) {
	 OtherClient* temp = getClientById(id);
	 if (!temp->gotPubKey())
	 {
		 cout << "cannot encrypt the symetric key" << endl;
		 return;
	 }
	 SendMessg payload;
	 memcpy_s(payload.cid, ID_LENGTH, id.c_str(), ID_LENGTH);
	 payload.type = SEND_SKEY_TYPE;
	 string sym_key;
	 if (!temp->gotSymeKey()) 
		 sym_key=createSkey(id);
	 sym_key = str_initizalization(temp->getSymetricKey(), SYM_KEY_LENGTH);
	 string content = AsymetricEncrypt::encryptMessage(temp->getPublicKey(),sym_key);
	 if (content.size() == 0)//encryption failed
		 return;
	 payload.cont_size = content.size();	
	 setHeaderReq(SEND_MESS_REQ, sizeof(SendMessg) + payload.cont_size);
	 if (!conn.connect()) return;
	 if (!conn.sendRequest((unsigned char*)&header_req, sizeof(header_req))) return;
	 if (!conn.sendRequest((unsigned char*)&payload, sizeof(SendMessg))) return;
	 if (!conn.sendRequest((unsigned char*)content.c_str(),payload.cont_size)) return;
	 SendMessageAns(sym_key, SEND_SKEY_TYPE,id);

 }

 /*
 * recive message text from the user and return it as a string
 */
 string Chat::getTextMessgFromUser() {
	 cout << "Enter your message:" << endl;
	 cout << "<press enter when line is empty to send>" << endl;
	 string in;
	 string out = "";
	 bool flag = true;
	 while (flag) {
		 getline(cin, in);
		 if (in.length() == 0)
			 flag = false;
		 if(flag)
			 out.append(in+'\n');
	 }
	 if (out.size() == 0)
		 out.append("\t");//we not allow to send a empty message, this fact help me to check if the decryption failed or succeed
	 return out;
 }

 //gets a name from the user input
 string Chat::getUserNameFromInput(string instruction) {
	 cout << instruction << endl;
	 string name;
	 cin >> name;
	 std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	 return name;
 }

 /*
 * handles sending encrypted text message
 */
 void Chat::sendTextMessPacket(string id) {
	 OtherClient* temp = getClientById(id);
	 if (!temp->gotSymeKey()) {//havn't the symetric key for this user
		 cout << "cannot encrypt messages for this user, request for his symetric key first." << endl;
		 return;
	 }
	 SendMessg payload;
	 string messg=getTextMessgFromUser();
	 messg = SymetricCrypt::encryptMessage(messg,temp->getSymetricKey());//encrypts the message
	 if (messg.size() == 0)//encryption failed
		 return;
	 if (messg.size() > _4MB)// welimit the size of the message to 4 megabyte
	 {
		 cout << "Warning:the message is to long, cannot complete the send" << endl;
		 return;
	 }
	 payload.cont_size = messg.size();
	 setHeaderReq(SEND_MESS_REQ, sizeof(SendMessg)+ payload.cont_size);
	 memcpy_s(payload.cid, ID_LENGTH, id.c_str(), ID_LENGTH);
	 payload.type = TEXT_MESS;
	 if (!conn.connect()) return;
	 if (!conn.sendRequest((unsigned char*)&header_req, sizeof(header_req))) return;
	 if (!conn.sendRequest((unsigned char*)&payload, sizeof(SendMessg))) return;
	 if (!conn.sendRequest((unsigned char*)messg.c_str(),payload.cont_size)) return;
	 SendMessageAns(messg, TEXT_MESS,id);

 }

 /*
 * handles the reply for send messages
 */
 void Chat::SendMessageAns(string content ,size_t type,string id) {
	 MessgSent recv;
	 if (!handleHeaderRecv(SEND_MESS_ANS))
		 return;
	 if (!handleAnsRecv((unsigned char*)&recv, (sizeof(recv.messg_id) + ID_LENGTH)))
		 return;
	 cout << "message sent successfully!" << endl;
	 if (type == SEND_SKEY_TYPE)
		 getClientById(id)->setSymetricKey((unsigned char*)content.c_str());//saves the symetric key for the requested client
 }

 //generate a symetric key of length 16 and returns it as a string
string Chat::createSkey(string id) {
	 //here i have to create the skey- crypto
	 CryptoPP::byte key[CryptoPP::AES::DEFAULT_KEYLENGTH];
	 memset(key, 0x00, CryptoPP::AES::DEFAULT_KEYLENGTH);
	 SymetricCrypt::generate_key(reinterpret_cast<char*>(key), CryptoPP::AES::DEFAULT_KEYLENGTH);
	 string symkey = str_initizalization(key,SYMETRIC_KEY_LENGTH);
	 return symkey;
}

//finds a client from the client list by is name and return him, if didnt found returns nullptr
 OtherClient* Chat::getClientByName(string name) {
	for (size_t i = 0; i < clients_list.size(); i++)
		 if (clients_list[i].getName() == name)
			 return &clients_list[i];
	return nullptr ;	 
 }

 //finds a client from the client list by is id and return him, if didnt found returns nullptr
 OtherClient* Chat::getClientById(string id) {
	 for (size_t i = 0; i < clients_list.size(); i++)
		 if (clients_list[i].getId() == id)
			 return &(clients_list[i]);
	 return nullptr;
 }

 
