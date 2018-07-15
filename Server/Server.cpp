#include <mutex>
#include <algorithm> 

#include "Server.h"
#include "myLog.h"
extern myLog LOG;

//static members declaration
Handler										Server::m_handle;
std::vector<std::shared_ptr<SOCKET>>		Server::m_clients;

Server::Server()
	:m_ipAddress(DEFAULT_IP_ADDRESS), m_port(DEFAULT_PORT) { }
Server::~Server() { CleanUp(); }


//Initialize winsock
bool Server::Init() {
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

//Main processing loop
void Server::Run() {

	//Create a listening socket
	auto listening = CreateSocket();
	std::cout << "\n\tNo one is connected to the server!";

	//Create a thread that read messages from Handler and send them to clients if mach 
	std::thread send_messages(&Server::sendMessageToClient);
	send_messages.detach();

	while (true) {
		if (listening != INVALID_SOCKET) {

			//Create a client socket
			std::shared_ptr<SOCKET> client;
			try {
				client = std::make_shared<SOCKET>(CreateClientSocket(listening));
			}
			catch (...) {
				LOG.write("... exception catch in Server::Run from std::shared_ptr<SOCKET>", myLog::Level::LevelWarning);
			}

			for (auto i : m_clients)
				Server::Send(*client, std::to_string(*i));

			for (auto i : m_clients)
				Server::Send(*i, "Client " + std::to_string(*client) + " has just connected!");

			try {
				m_clients.push_back(client);
			}
			catch (const std::bad_alloc &) {
				LOG.write("bad_alloc exception catch in Server::Run from push_back", myLog::Level::LevelWarning);
			}
			catch (const std::exception &) {
				LOG.write("std::exception catch in Server::Run from push_back", myLog::Level::LevelWarning);
			}
			catch (...) {
				LOG.write("... exception catch in Server::Run from push_back", myLog::Level::LevelWarning);
			}

			if (*client != INVALID_SOCKET) {
				//START new Thread
				Create_Thread(client);

			}
			else {
				LOG.write("INVALIDE client socket Server::Run", myLog::Level::LevelWarning);
				closesocket(*client);
			}
		} 
		else {
			closesocket(listening);
			LOG.write("INVALIDE listening socket Server::Run", myLog::Level::LevelError);
			listening = CreateSocket();
		}
	}

	
	
}

//Create a socket
SOCKET Server::CreateSocket() {
	SOCKET listening = socket(AF_INET, SOCK_STREAM, NULL);
	if (listening != INVALID_SOCKET) {
		sockaddr_in hint;
		int hintSize = sizeof(hint);
		hint.sin_family = AF_INET;
		hint.sin_port = htons(m_port);
		inet_pton(AF_INET, m_ipAddress, &hint.sin_addr);
		LOG.write("hint sock has been created", myLog::Level::LevelInfo);

		int bindOk = bind(listening, (sockaddr*)&hint, sizeof(hint));
		if (bindOk != SOCKET_ERROR) {
			LOG.write("The bind bettwin listening socket and hint socket is done", myLog::Level::LevelInfo);
			int listenOk = listen(listening, SOMAXCONN);

			if (listenOk == SOCKET_ERROR) {
				LOG.write("An error has ocure while trying to initializated listenOk", myLog::Level::LevelError);
				return -1;
			}
			else {
				LOG.write("listenOk != SOCKET_ERROR", myLog::Level::LevelInfo);
			}
		}
		else {
			LOG.write("An error has ocure while trying to initializated bindOk", myLog::Level::LevelError);
			return -1;
		}

	}
	else {
		LOG.write("An error has ocure while trying to SOCKET listening", myLog::Level::LevelError);
		return -1;
	}
	return listening;
}

//Wait for a connection
SOCKET Server::CreateClientSocket(SOCKET listening) {

	SOCKET client;
	client = accept(listening, NULL, NULL);
	if (client == SOCKET_ERROR) {
		LOG.write("Client socket = SOCKET_ERROR", myLog::Level::LevelError);
	}
	else {
		LOG.write("Client socket is OK", myLog::Level::LevelInfo);
	}

	return client;
}

//Start new thread with every client and store it in m_threadsClients
void Server::Create_Thread(std::shared_ptr<SOCKET>& client) {

	SOCKET temp = *client;
	if (temp != SOCKET_ERROR) {

		std::thread temp_thread(&Server::connectClient, temp);
		try {
			m_threadClients.push_back(std::move(temp_thread));
		}
		catch (const std::bad_alloc &) {
			LOG.write("bad_alloc exception catch in Server::Create_Thread from push_back", myLog::Level::LevelWarning);
		}
		catch (const std::exception &) {
			LOG.write("std::exception catch in Server::Create_Thread from push_back", myLog::Level::LevelWarning);
		}
		catch (...) {
			LOG.write("... exception catch in Server::Create_Thread from push_back", myLog::Level::LevelWarning);
		}
		LOG.write("New thread has started!", myLog::Level::LevelInfo);

	}
	else
		LOG.write("temp == SOCKET_ERROR", myLog::Level::LevelError);

}

//Cleanup and wait for every thread to finish
void Server::CleanUp() {
	for (auto&& x : m_threadClients) {
		x.join();
	}
	m_threadClients.clear();
	m_clients.clear();
	WSACleanup();
	LOG.write("Server stopped running!", myLog::Level::LevelInfo);
}

//Recive and send messages to client
void Server::connectClient(SOCKET client) {

	std::cout << "\n\tA connection was found! " << client;

	int bytesRecived = 0;
	std::mutex messageSend_mutex;
	char buffer[MAX_BUFFER_SIZE];

	while (true) {
		ZeroMemory(buffer, MAX_BUFFER_SIZE);				//memset (buffer, 0, MAX_BUFFER_SIZE)
		if (bytesRecived = recv(client, buffer, MAX_BUFFER_SIZE, 0)) {

			messageSend_mutex.lock();
			m_handle.SetMessage(static_cast<std::string>(buffer));
			messageSend_mutex.unlock();

		}
		else {
			LOG.write("Didn't recive any message!", myLog::Level::LevelInfo);
		}
	}
}

//Send message to the specified client
void Server::sendMessageToClient() {

	std::mutex messageRecive_mutex;
	using namespace std::chrono_literals;

	while (true) {

		try { std::this_thread::sleep_for(100ms); }
		catch (...) {
			LOG.write("... exception catch in Server::sendMessageToClient at sleep_for", myLog::Level::LevelWarning);
		}

		messageRecive_mutex.lock();
		std::pair<int, std::string> msg = m_handle.getMessage();
		messageRecive_mutex.unlock();
	
		if (msg.first != -1 && msg.second != "-1" && search_client(msg.first)) {	
			Server::Send(msg.first, std::to_string(msg.first) + msg.second);
		}
		
	}

}

//Send message to the specified client
void Server::Send(int clientSocket, std::string msg) {

	if (send(clientSocket, msg.c_str(), msg.size() + 1, 0) == SOCKET_ERROR) {
		LOG.write("Error while trying to send the message", myLog::Level::LevelError);
	}
	else {
		auto temp = "msg sent to the client " + std::to_string(clientSocket);
		LOG.write(temp.c_str(), myLog::Level::LevelInfo);
	}
}

//search for client in the clients vector
bool Server::search_client(int client) {
	for (auto i : m_clients)
		if (static_cast<int>(*i) == client)
			return true;
	return false;
}