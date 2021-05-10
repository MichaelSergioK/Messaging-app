#include "AsymetricEncrypt.h"


using namespace std;


//generate a private key of 1024 bits
std::string AsymetricEncrypt::generatePrivateKey() {
	try {
		CryptoPP::AutoSeededRandomPool rng;
		CryptoPP::InvertibleRSAFunction privkey;
		privkey.Initialize(rng, 1024);
		return privateKeyToString(privkey);
	}
	catch (exception& e) {
		cout << "couldn't generate the private key"  << endl;
		return "";
	}

}

//generate a public key from privkey
std::string AsymetricEncrypt::generatePublicKey(std::string privkey)
{
	try {
		std::string key_to_return;
		CryptoPP::RSA::PrivateKey private_key = privateKeyFromString(privkey);
		CryptoPP::RSAFunction  pubkey(private_key);
		CryptoPP::Base64Encoder pubkeysink(new CryptoPP::StringSink(key_to_return));
		pubkey.DEREncode(pubkeysink);
		pubkeysink.MessageEnd();
		return key_to_return;
	}
	catch (exception& e) {
		cout << "couldn't generate the public key" << endl;
		return "";
		
	}
}

/*cut the public key-to contain 160 chars- 
 * We use 160 character here becuase our key is 1024 bits in size (the actual public key contains more data then just "the key")
 */
void AsymetricEncrypt::cutPubKey(std::string pubkey,unsigned char* buf)
{
	CryptoPP::RSAFunction pubKey = pubKeyFromString(pubkey);
	CryptoPP::ArraySink as(buf, KEY_L);
	pubKey.Save(as);
}


/*
* returns the string representation of key, if not success- returns empty string.
*/
std::string AsymetricEncrypt::privateKeyToString(CryptoPP::RSA::PrivateKey& key)
{
	try {
		std::string key_to_return;
		CryptoPP::Base64Encoder privkeysink(new CryptoPP::StringSink(key_to_return));
		key.DEREncode(privkeysink);
		privkeysink.MessageEnd();
		return key_to_return;
	}
	catch (exception& e) {
		cout << "couldn't encode the private key "  << endl;
		return "";

	}
}

/*
* returns the string representation of key, if not success- returns empty string.
*/
std::string AsymetricEncrypt::pubicKeyToString(CryptoPP::RSAFunction& key)
{
	try {
		std::string key_to_return;
		CryptoPP::Base64Encoder privkeysink(new CryptoPP::StringSink(key_to_return));
		key.DEREncode(privkeysink);
		privkeysink.MessageEnd();
		return key_to_return;
	}
	catch (exception& e) {
		cout << "couldn't encode the public key "  << endl;
		return "";

	}
}


/*
* extracts the private key from the encoded key represented by the string key
*/
CryptoPP::RSA::PrivateKey AsymetricEncrypt::privateKeyFromString(std::string key)
{
	try {
		CryptoPP::ByteQueue bytes;
		CryptoPP::StringSource str(key, true, new CryptoPP::Base64Decoder);
		str.TransferTo(bytes);
		bytes.MessageEnd();
		CryptoPP::RSA::PrivateKey key_to_return;
		key_to_return.Load(bytes);
		return key_to_return;
	}
	catch (exception& e) {
		cout << "couldn't extract the private key: "  << endl;
	}
}
/*
* extracts the public key from the encoded key represented by the string key
*/
CryptoPP::RSAFunction AsymetricEncrypt::pubKeyFromString(std::string key)
{
	try {
		CryptoPP::ByteQueue bytes;
		CryptoPP::StringSource str(key, true, new CryptoPP::Base64Decoder);
		str.TransferTo(bytes);
		bytes.MessageEnd();
		CryptoPP::RSA::PublicKey key_to_return;
		key_to_return.Load(bytes);
		return key_to_return;
	}
	catch (exception& e) {
		cout << "couldn't extract the public key: "  << endl;
	}
}


/*encrypt message with private key represented by privkey_str. returns the encrypted message in case of success,
otherwise - empty string*/
std::string AsymetricEncrypt::encryptMessage(CryptoPP::byte* buf, std::string message)
{
	try {
		CryptoPP::AutoSeededRandomPool rng;
		CryptoPP::ArraySource as(buf, KEY_L, true);
		CryptoPP::RSA::PublicKey pubKey;
		pubKey.Load(as);
		std::string ciphertext;
		CryptoPP::RSAES_OAEP_SHA_Encryptor e(pubKey);
		CryptoPP::StringSource ss(message, true, new CryptoPP::PK_EncryptorFilter(rng, e, new CryptoPP::StringSink(ciphertext)));
		return ciphertext;
	}
	catch (exception& e) {
		cout << "couldn't encrypt the key: "  << endl;
		return "";
	}
}

/*decrypts message with private key represented by privkey_str. returns the decrypted message in case of success,
otherwise - empty string*/
std::string AsymetricEncrypt::decryptMessage(std::string privkey_str, std::string message)
{
	try {
		CryptoPP::AutoSeededRandomPool rng;
		CryptoPP::RSA::PrivateKey privkey = privateKeyFromString(privkey_str);
		std::string decrypted;
		CryptoPP::RSAES_OAEP_SHA_Decryptor d(privkey);
		CryptoPP::StringSource ss(message, true, new CryptoPP::PK_DecryptorFilter(rng, d, new CryptoPP::StringSink(decrypted)));
		return decrypted;
	}
	catch (exception& e) {
		cout << "couldn't decrypt the key" << endl;
		return "";

	}
}




