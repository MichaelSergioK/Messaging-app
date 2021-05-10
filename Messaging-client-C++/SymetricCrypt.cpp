#include "SymetricCrypt.h"
using namespace std;
/*
	generates symetric key-
	intialize buff with the symetric key
	 size must be a 4 bytes align.
*/
char* SymetricCrypt::generate_key(char* buff, size_t size)
{
	
		for (size_t i = 0; i < size; i += 4)
			_rdrand32_step(reinterpret_cast<unsigned int*>(&buff[i]));
		return buff;
}


/*
	encrypt message with key and returns the encrypted message
*/
std::string SymetricCrypt::encryptMessage(std::string message, unsigned char* key)
{
	try {
		CryptoPP::byte iv[CryptoPP::AES::BLOCKSIZE];
		memset(iv, 0x00, CryptoPP::AES::BLOCKSIZE);
		std::string ciphertext;
		CryptoPP::AES::Encryption aesEncryption(key, CryptoPP::AES::DEFAULT_KEYLENGTH);
		CryptoPP::CBC_Mode_ExternalCipher::Encryption cbcEncryption(aesEncryption, iv);
		CryptoPP::StreamTransformationFilter stfEncryptor(cbcEncryption, new CryptoPP::StringSink(ciphertext));
		stfEncryptor.Put(reinterpret_cast<const unsigned char*>(message.c_str()), message.length());
		stfEncryptor.MessageEnd();
		return ciphertext;
	}
	catch (exception& e) {
		cout << "couldnt encrypt the message: " << e.what() << endl;
		return "";
	}
}


/*
decrypt message with key
and returns the decrypted message
*/
std::string SymetricCrypt::decryptMessage(std::string ciphertext, unsigned char* key)
{
	try {
		CryptoPP::byte iv[CryptoPP::AES::BLOCKSIZE];
		memset(iv, 0x00, CryptoPP::AES::BLOCKSIZE);
		std::string decryptedtext;

		CryptoPP::AES::Decryption aesDecryption(key, CryptoPP::AES::DEFAULT_KEYLENGTH);
		CryptoPP::CBC_Mode_ExternalCipher::Decryption cbcDecryption(aesDecryption, iv);

		CryptoPP::StreamTransformationFilter stfDecryptor(cbcDecryption, new CryptoPP::StringSink(decryptedtext));
		stfDecryptor.Put(reinterpret_cast<const unsigned char*>(ciphertext.c_str()), ciphertext.size());
		stfDecryptor.MessageEnd();
		return decryptedtext;
	}
	catch (exception& e) {
		cout << "couldnt decrypt the message: " << e.what() << endl;
		return "";
	}
}
