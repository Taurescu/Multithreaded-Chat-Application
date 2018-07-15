#pragma once
#include <WS2tcpip.h>					//Header file for Winsock functions
#pragma comment(lib, "ws2_32.lib")		//Winsock library file

#include <thread>
#include "Handler.h"

#define DEFAULT_IP_ADDRESS			"127.0.0.1"
#define DEFAULT_PORT				444
#define MAX_BUFFER_SIZE				49152

//Forward declaration of class
class Client;

class Client
{
public:
	Client();
	~Client();

	//Initialize winsock
	bool init();
	
	//Main procesing loop
	void run();
	
private:

	//Create socket
	SOCKET createSocket();

	//Start new thread for receiving messages
	void createThread(SOCKET connection);
	
	//Send message
	void sendMessage(SOCKET connection, char* buffer);

	//Recive message
	static void reciveMessage(SOCKET connection);

	//Clean up
	void CleanUp();

private:

	std::thread							m_thread;
	const char*							m_ipAddress;
	int									m_port;
};

