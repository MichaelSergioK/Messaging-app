#include  "Chat.h"

int main() {
	boost::asio::io_context io_context;
	tcp::socket s(io_context);
	tcp::resolver resolver(io_context);
	Chat chat(s, resolver);
	for (; ; ) {
		int choice;
		std::cout << "1)Register\n2)Request for client list\n3)Request for public key\n4)Request for waiting messages\n5) Send a text message\n51)Send a request for symetric key\n52)Send your symetric key\n0)Exit client" << std::endl;
		std::cin >> choice;
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');//ignore the rest of input
		switch (choice) {
		case 1:chat.RegisterReq(); break;
		case 2:chat.ClientsListReq();  break; 
		case 3:chat.publicKeyReq(); break; 
		case 4:chat.WaitingMessagesReq(); break;
		case 5:chat.SendMessageReq(TEXT_MESS); break;
		case 51:chat.SendMessageReq(REQUEST_FOR_SKEY_TYPE); break;
		case 52:chat.SendMessageReq(SEND_SKEY_TYPE); break;
		case 0:exit(1);
		default:break;
		

		}
	}
}
	