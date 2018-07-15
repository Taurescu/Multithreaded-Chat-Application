#include "Client.h"
#include <iostream>

#include "myLog.h"
extern myLog LOG;


Client::Client()
	:m_ipAddress(DEFAULT_IP_ADDRESS), m_port(DEFAULT_PORT) { }
Client::~Client()
{
	CleanUp();
}


//Initialize winsock
bool Client::init() {
	WSAData data;
	WORD ver = MAKEWORD(2, 1);

	int wsInit = WSAStartup(ver, &data);
	if (wsInit == 0) {
		LOG.write("Succesfull initialization of wsInit", myLog::Level::LevelInfo);
		return true;
	}
	else {
		LOG.write("Unsuccesfull initialization of wsInit", myLog::Level::LevelInfo);
		return false;
	}
}

//Main procesing loop
void Client::run() {

	auto connection = createSocket();
	createThread(connection);

	while (true) {
		if (connection != SOCKET_ERROR) {

			char buffer[MAX_BUFFER_SIZE];
			ZeroMemory(buffer, MAX_BUFFER_SIZE);

			std::cin.clear();
			std::cin.sync();
			std::cin.getline(buffer, sizeof(buffer));
			m_handlerSend.setConversation(static_cast<std::string>(buffer));
			sendMessage(connection,buffer)

		}
		else {
			LOG.write("INVALIDE connection in Client::Run", myLog::Level::LevelError);
			closesocket(connection);
			connection = createSocket();
		}
	}
}

//Create and connect  socket 
SOCKET Client::createSocket() {
	SOCKET sConnect = socket(AF_INET, SOCK_STREAM, NULL);
	if (sConnect != SOCKET_ERROR) {
		sockaddr_in hint;
		int hintSize = sizeof(hint);
		hint.sin_family = AF_INET;
		hint.sin_port = htons(m_port);
		inet_pton(AF_INET, m_ipAddress, &hint.sin_addr);
		LOG.write("hint sock has been created", myLog::Level::LevelInfo);

		int connectOk = connect(sConnect, (SOCKADDR*)(&hint), sizeof(hint));
		if (connectOk != SOCKET_ERROR) {
			LOG.write("Successfully connected", myLog::Level::LevelInfo);
			return sConnect;
		}
		else
			LOG.write("Error occurred while trying to connect", myLog::Level::LevelError);
	}
	return SOCKET_ERROR;
}

//Start new thread for receiving messages
void Client::createThread(SOCKET connection) {
	if (connection != SOCKET_ERROR) {
		m_thread = std::thread(&Client::reciveMessage, connection);
		LOG.write("New thread has started!", myLog::Level::LevelInfo);
	}
	else
		LOG.write("connection == SOCKET_ERROR", myLog::Level::LevelError);
}

//Send message
void Client::sendMessage(SOCKET connection, char* buffer){
	int msgSent;
	HandlerSend	m_handlerSend;
	if (connection != SOCKET_ERROR) {
		while (true) {
			if (msgSent = send(connection, m_handlerSend.send(buffer), m_handlerSend.size(), NULL));
			std::cin.getline(buffer, MAX_BUFFER_SIZE);
		}
	}

}

//Recive message
void Client::reciveMessage(SOCKET connection){
	int recived = 0;
	char buffer[MAX_BUFFER_SIZE];
	HandlerRecv	m_handleRecive;

	while (true) {
		ZeroMemory(buffer, MAX_BUFFER_SIZE);
		if (recived = recv(connection, buffer, MAX_BUFFER_SIZE, 0)) {
			if (buffer[0] != 0) {
				m_handleRecive.recive(static_cast<std::string>(buffer));
			}
		}
	}

}

//Clean up
void Client::CleanUp() {
	m_thread.join();
	WSACleanup();
	LOG.write("Client has stop running!", myLog::Level::LevelInfo);
}