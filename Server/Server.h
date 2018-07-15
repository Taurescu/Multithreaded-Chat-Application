#pragma once
#include <WS2tcpip.h>					//Header file for Winsock functions
#pragma comment(lib, "ws2_32.lib")		//Winsock library file

#include <vector>
#include <memory>
#include <thread>
#include "Handler.h"

#define DEFAULT_IP_ADDRESS			"127.0.0.1"
#define DEFAULT_PORT				444
#define MAX_BUFFER_SIZE				49152

//Forward declaration of class
class Server;

class Server
{
public:
	Server();
	~Server();

	//Initialize winsock
	bool Init();

	//Main procesing loop
	void Run();

private:

	//Create a socket
	SOCKET CreateSocket();

	//Create client socket and wait for a connection
	SOCKET CreateClientSocket(SOCKET listening);

	//Start new thread with every client and store it in clients
	void Create_Thread(std::shared_ptr<SOCKET>& client);

	//Cleanup and wait for every thread to finish
	void CleanUp();

	//Client connection run by a detach thread
	static void connectClient(SOCKET client);

	//Send messages to the specific client
	static void sendMessageToClient();

	//Send message to the specified client
	static void Send(int clientSocket, std::string msg);

	//search for client in the clients vector
	static inline bool search_client(int client);



private:

	std::vector<std::thread>						m_threadClients;
	static std::vector<std::shared_ptr<SOCKET>>		m_clients;
	static Handler									m_handle;
	const char*										m_ipAddress;
	int												m_port;

};




