#include "Client.h"
#include "myLog.h"
myLog LOG;

int main() {

	Client *client = new Client;

	if (client->init())
		client->run();
	else
		LOG.write("Can't start the Server in main function!", myLog::Level::LevelError);

	delete(client);
	return 0;
}