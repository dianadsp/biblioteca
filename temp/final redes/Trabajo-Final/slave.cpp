#include <iostream>
#include "src/slave.h"

//g++ slave.cpp -o slave -std=c++11 -pthread -lsqlite3
//./slave port 127.0.0.1 slave(poner_numero).db
//EJEMPLO: ./slave 8012 127.0.0.1 slave5.db

using namespace std;

int main(int argc, char const *argv[]){
	int port;
	string ip,db;
	if(argc == 4){
		port = stoi(argv[1]);
		ip = argv[2];
		db = "db/" + string(argv[3]);
	}

	CSlave *slave = new CSlave(port, ip, db);
	slave->inicializar();

	delete slave;
	return 0;
}