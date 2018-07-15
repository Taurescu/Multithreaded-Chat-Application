#include "myLog.h"
#include <chrono>
#include <iomanip>


myLog::myLog() {
	initVars();
}
myLog::~myLog() {

	log_file.open("systemLog.txt", std::ios::out | std::ios::out | std::ios::app);

	if (log_file.fail()) {
		std::cout << "Unable to process or open the file !" << std::endl;
		exit(EXIT_FAILURE);
	}
	else {
		printHeader(1);
	}
	log_file.close();
}

//Write message in the file with Default level
void myLog::write(const char* msg) {
	m_LogLevel = myLog::Level::LevelInfo;
	log_message = msg;
	init();
}

//Write message in the file and select the level
void myLog::write(const char * msg, Level level) {
	m_LogLevel = level;
	log_message = msg;
	init();
}

//initiate Variables
void myLog::initVars() {
	log_message = "0";
	m_LogLevel = myLog::Level::LevelInfo;
	now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

}

//Initiate the open file
void myLog::init() {
	openLog("systemLog.txt");
}

//Open the file log and write into it
void myLog::openLog(const std::string& filename) {

	log_file.open(filename.data(), std::ios::out | std::ios::out | std::ios::app);

	if (log_file.fail()) {
		std::cout << "Unable to process or open the file !" << std::endl;
		exit(EXIT_FAILURE);
	}
	else {
		log_file.seekp(0, std::ios::end);

		if (!log_file.tellp()) {
			printHeader(0);
		}

		now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		log_file << '[' << m_LogLevel << "]:(" << log_message << "), at:" << std::put_time(localtime(&now), "%F %T") << "\n";

	}
	log_file.close();
}

//Print the header when the file is empty or the class is closed
void myLog::printHeader(int type) {
	switch (type) {
	case 0:
		now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		log_file << "Start of messages at: " << std::put_time(localtime(&now), "%F %T") << "\n";
		break;
	case 1:
		now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		log_file << "The class has been closed at: " << std::put_time(localtime(&now), "%F %T") << "\n\n";
		break;
	}
}

//Outside the class function
//Overload outstream << to cout based on the enum Level
std::ostream& operator<<(std::ostream& out, myLog::Level level) {
	switch (level) {
	case(myLog::Level::LevelInfo):
		out << "Info";
		break;
	case(myLog::Level::LevelWarning):
		out << "Warning";
		break;
	case(myLog::Level::LevelError):
		out << "Error";
		break;
	}
	return out;
}