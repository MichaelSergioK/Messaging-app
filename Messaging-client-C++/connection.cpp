
#include "connection.h"
using namespace std;

ConnHandler::ConnHandler( tcp::socket& s, tcp::resolver& resolver)
	:sock(s),resolver(resolver),port(""),ip("") {
}

//connect to server- returns true in case of success ,otherwise returns false
bool ConnHandler::connect() {
	try {
		boost::asio::connect(sock, resolver.resolve(ip, port));
		return true;
	}
	catch (const boost::system::system_error& ex) {
		cout << "error:connection to server failed "<< endl;
		return false;
	}
}
//disconnect from server
void ConnHandler::disconnect() {
	if(sock.is_open())
		sock.close();
}
//send a request of size 'size' to the server- returns true in case of success ,otherwise returns false

bool ConnHandler::sendRequest(unsigned char *request, size_t size) {
	try {
		boost::asio::write(sock, boost::asio::buffer(request,
			size));
		return true;
	}
	catch (boost::system::system_error& ex) {
		cout << "error:send failed " << endl;
		return false;
	}
}


// recive a request of size 'size' from server, returns true in case of success ,otherwise returns false
//saves the request in a unsigned char buffer
bool ConnHandler::recvRequest(unsigned char* recv_buff, size_t size)
{
	try {
		boost::asio::read(sock, boost::asio::buffer(recv_buff,
			size));
		return true;
	}
	catch (boost::system::system_error& ex) {
		cout << "error:recive failed " << endl;
		return false;
	}
}

// recive a request of size 'size' from server, returns true in case of success ,otherwise returns false
//saves the request in a vector

bool ConnHandler::recvRequest(vector <uint8_t> recv_buff, size_t size)
{
	try {
		boost::asio::read(sock, boost::asio::buffer(recv_buff.data(),
			size));
		return true;
	}
	catch (boost::system::system_error& ex) {
		cout << "error:recive failed " << endl;
		return false;
	}
}

	string ConnHandler::getIp() {
		return ip;
	}
	string ConnHandler::getPort() {
		return port;
	}
	void ConnHandler::setIp(string tmpip) {
		ip = tmpip;
	}
	void ConnHandler::setPort(string tmpPort) {
		port = tmpPort;
	}
	ConnHandler::~ConnHandler(){
			disconnect();
		}

	




