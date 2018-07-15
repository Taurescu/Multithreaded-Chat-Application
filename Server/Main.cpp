#include "Server.h"
#include "myLog.h"
myLog LOG;

int main() {

	Server* server = new Server;
	if (server->Init()) {
		server->Run();
	}
	else {
		LOG.write("Can't start the Server in main function!", myLog::Level::LevelError);
	}
	delete server;

	return 0;
}
