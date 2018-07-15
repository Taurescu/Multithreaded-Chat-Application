#pragma once
#include <iostream>
#include <fstream>


class myLog
{
public:
	enum class Level { LevelInfo, LevelWarning, LevelError };

	myLog();
	~myLog();

	//Write message in the file with Default level
	void write(const char* msg);

	//Write message in the file and select the level
	void write(const char* msg, Level level);
	

private:

	//initiate Variables
	void initVars();

	//Initiate the open file
	void init();

	//Open the file log and write into it
	void openLog(const std::string&);

	//Print the header when the file is empty or the class is closed
	void printHeader(int);

private:

	Level					m_LogLevel;
	time_t					now;	
	const char*				log_message;
	std::fstream			log_file;

};

std::ostream& operator<<(std::ostream& out, myLog::Level);
//extern myLog LOG;