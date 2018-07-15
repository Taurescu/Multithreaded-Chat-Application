#pragma once
#include <string>
#include <queue>


class Handler {
public:
	Handler();
	~Handler();

	//Recive Message
	void SetMessage(std::string msg);

	//Get message 
	std::pair<int, std::string> getMessage();

private:
	//Find and return the name from string
	int find_name(std::string& msg);

	//Find and return the message content from string
	//std::string find_msg(std::string& msg);

private:
	std::queue<std::pair<int, std::string>>		 m_msg;
};

