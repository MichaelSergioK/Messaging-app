#pragma once
#include <cstdlib>
#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <vector>

using boost::asio::ip::tcp;
/*
this class is responsible to handle the connection with the server
*/
class ConnHandler {
	std::string port;
	std::string ip;
	tcp::socket &sock;
	tcp::resolver& resolver;

public:
	ConnHandler(tcp::socket& sock, tcp::resolver& resolver);
	void disconnect();
	bool sendRequest(unsigned char* request, size_t size);
	bool recvRequest(unsigned char* recv_buff, size_t size);
	bool recvRequest(std::vector <uint8_t> recv_buff, size_t size);
	bool connect();
	std::string getIp();
	std::string getPort();
	void setIp(std::string tmpip);
	void setPort(std::string tmpPort);
	virtual ~ConnHandler();
	

};