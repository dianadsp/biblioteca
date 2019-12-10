#include <iostream>
#include "src/client.h"

//g++ client.cpp -o client -std=c++11 -pthread -lsqlite3
//./client

using namespace std;

int main(int argc, char const *argv[]){
	int port = 6666;
	string ip = "127.0.0.1";

	CClient *client = new CClient(port, ip);
	client->inicializar();

	delete client;
	return 0;
}