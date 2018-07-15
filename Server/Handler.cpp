#include <cctype>
#include "Handler.h"
#include "myLog.h"					
extern myLog LOG;

Handler::Handler() {}
Handler::~Handler(){}

//Recive Message
void Handler::SetMessage(std::string msg)
{
	try {
		m_msg.push(std::make_pair(find_name(msg), msg));
	}
	catch (const std::bad_alloc &) {
		LOG.write("bad_alloc exception catch in Handler::SetMessage from push", myLog::Level::LevelWarning);
	}
	catch (const std::exception &) {
		LOG.write("std::exception exception catch in Handler::SetMessage from push", myLog::Level::LevelWarning);
	}
	catch (...) {
		LOG.write("... exception catch in Handler::SetMessage from push", myLog::Level::LevelWarning);
	}

}

//Get message 
std::pair<int, std::string> Handler::getMessage() {
	std::pair<int, std::string> temp(-1, "-1");
	if (!m_msg.empty()) {
		try {
			temp = m_msg.front();
		}
		catch (...) {
			LOG.write("... Exception catch in Handler::getMessage from front() ", myLog::Level::LevelWarning);
		}

		try {
			m_msg.pop();
		}
		catch (...) {
			LOG.write("... Exception catch in Handler::getMessage from pop() ", myLog::Level::LevelWarning);
		}
	}
	return temp;
}

//Find and return the name from string
int Handler::find_name(std::string& msg) {
	auto found = msg.find('#');
	int res = -1;
	if (found != std::string::npos && found + 3 < msg.size()) {
		if (std::find_if((msg.begin() + found + 1), (msg.begin() + found + 4), [](char c) {return !std::isdigit(c); }) == (msg.begin() + found + 4)) {
			res = std::stoi(msg.substr((found + 1), 3));
			msg.erase(found, 4);
		}
	}

	return res;
}


//Find and return the message content from string
//std::string Handler::find_msg(std::string& msg) {
//	auto found = msg.find('#');
//	if (found == std::string::npos) {
//		return "-1";
//	}
//	return msg;
//}

