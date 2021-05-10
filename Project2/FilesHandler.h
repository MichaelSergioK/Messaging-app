#pragma once
#include <fstream>
#include <iostream>
#include<string>
#include <boost/asio.hpp>


//this class is used to handle the files in the application
class FileHandler {
	std::fstream fio;
	std::string fileName;
	public:
		bool exist(const std::string filename);
		FileHandler(std::string filename);
		~FileHandler();
		bool open();
		bool createFile(std::string name);
		void close();
		std::string readLine();
		bool writeLine(std::string line);
		void setFileName(std::string name);
		std::string getFileName();
		

};