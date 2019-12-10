#ifndef _CLIENT_H_
#define _CLIENT_H_

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <thread>
#include "lib.h"
using namespace std;
// extern unsigned rsize;
// extern unsigned csize;
int rsize = 30;
int csize = 60;

static char DIR = 'd';

void thread_write(int _sockFD){
	string text;

	while(true){
		getline(std::cin, text);
		// send(_sockFD, text.c_str(), text.size(), 0);
		DIR = text[0];
	}
}

void thread_write_speed(int _sockFD, int _speed){
	std::string text;

	while(true){
		// sleep(_speed);
		std::this_thread::sleep_for(std::chrono::milliseconds(_speed));
		
		text = std::string(1, DIR);
		send(_sockFD, text.c_str(), text.size(), 0);
	}
}

void thread_read(int _sockFD, unsigned _rsize, unsigned _csize){
	unsigned buffer_size = _rsize*_csize;
	char buffer[buffer_size];

	while(true){
		memset(&buffer, 0, buffer_size);
		if(recv(_sockFD, buffer, buffer_size, 0) > 0){
			system("clear");			
			print_table_from_str(std::string(buffer),_rsize,_csize);
		}
	}
}

class CClient{
private:
	int m_clientSock;
	struct sockaddr_in m_clientAddr;
public:
	CClient();
	void Connect(std::string, int);
	void Talking(int);
};

#endif