#include "FilesHandler.h"

using namespace std;

FileHandler::FileHandler(std::string fileName):fileName(fileName) {}


FileHandler::~FileHandler() {
	fio.close();
}

/*
open file- return true in case of sucsses ,
otherwise- file doesnt exists or cin case of failure in opening- return false.
*/

bool FileHandler::open(){
	try {
		if (!exist(fileName)) {
			throw logic_error("file doesnt exist");
		}
		fio.open(fileName, std::fstream::in | std::fstream::out | std::fstream::app);
		return true;
	}
	catch (const std::system_error& e) {
		cout << "error: could not open the file" + fileName<<endl; 
		return false;
	}
	catch (const logic_error& e) {
		cout << "error: file:" + fileName +" doesnt exist"<<endl;
		return false;
	}

}

//create a file named by name
bool FileHandler::createFile(string name) {
	try {
		
		fio.open(fileName, std::fstream::in | std::fstream::out | std::fstream::app);
		return true;
	}
	catch (const std::system_error& e) {
		cout << "error: could not open the file" + fileName << endl;
		return false;
	}

}

void FileHandler::close(){
	fileName = "";
	fio.close();

}

/*
read a line from the file and returns the line in case of success, if the line is empty or 
could not read it- returns an empty string
*/
std::string FileHandler:: readLine() {
	try {
		if (!fio.is_open())
		{
			throw logic_error("file is not open");
		}
		fio.seekg(0, std::ios::cur);
		string line="";
		std::getline(fio,line);
		return line;
	}

	catch (const exception& e ) {
		cout << "error: " << e.what()<<endl;
		return "";
	}

}

/*
write a line to file, returns true in case of success, otherwise- false
*/
bool FileHandler::writeLine(string line)
{
	try {
		if (!fio.is_open())
		{
			throw logic_error("file is not open");
		}
		fio << line;
		return true;
	}
	catch (const exception& e) {
		cout << "error: " << e.what() << endl;
		return false;
	}

}


//set a new file to work with, if another file is open with this file handler - it will be closed
void FileHandler::setFileName(string name) {
	fileName = name;
	if (fio.is_open())
		close();
}

string FileHandler::getFileName() {
	return fileName;
}
//check if the file with the name filename exist
bool FileHandler::exist(const string filename) {
	ifstream ifile;
	ifile.open(filename);
	if (ifile) {
		ifile.close();
		return true;
	}
	return false;
	/*if (boost::filesystem::exists(filename))
		return true;*/
	return false;
}


