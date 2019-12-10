#include <iostream>
#include "client.h"

// unsigned rsize = 10;
// unsigned csize = 50;

// build g++ -std=c++11 -pthread client.cpp -o client.out
using namespace std;


CClient::CClient(){
	this->m_clientSock = socket(AF_INET, SOCK_STREAM, 0);
}

void CClient::Connect(std::string _ip, int _port){
    if(m_clientSock < 0){
        perror("error: cannot create Socket!");
        exit(0);
    }

    memset(&m_clientAddr, 0, sizeof(m_clientAddr));
    m_clientAddr.sin_family = AF_INET;
    m_clientAddr.sin_port = htons(_port);
    // m_clientAddr.sin_addr.s_addr = inet_addr(_ip.c_str());
 
    int res = inet_pton(AF_INET, _ip.c_str(), &m_clientAddr.sin_addr);
 
    if (0 > res){
      perror("error: first parameter is not a valid address family");
      close(m_clientSock);
      exit(EXIT_FAILURE);
    }
    else if (0 == res){
      perror("error: char string (second parameter does not contain valid ipaddress");
      close(m_clientSock);
      exit(EXIT_FAILURE);
    }

    if (connect(m_clientSock, (const struct sockaddr *)&m_clientAddr, sizeof(m_clientAddr)) == -1){
        perror("error: connect failed"); 
        exit(1);
    }

    std::cout << "Client connected->\n";
}

void CClient::Talking(int _speed){
    std::string text;
    bool t = true;

    unsigned buffer_size = 2;
	char buffer[buffer_size];

	while(t){
		cout<<"Enter your avatar: ";
		getline(std::cin, text);
		send(m_clientSock, text.c_str(), text.size(), 0);
		
		if(recv(m_clientSock, buffer, buffer_size, 0) > 0){
			if(buffer[0] == '1'){
				// std::cout << "connected\n";
				t = false;
			}
			else if(buffer[0] == '0'){
				std::cout << "error: This avatar already exist!\n";
			}
		}
	}

    std::thread tread(thread_read,   m_clientSock, rsize, csize);
    std::thread twrite(thread_write, m_clientSock);
    std::thread twrites(thread_write_speed, m_clientSock, _speed);

    tread.join();
    twrite.join();
    twrites.join();

    close(m_clientSock);
}

int main(int argc, char const *argv[]){
		
	//std::string ip = "192.168.1.41";	
	std::string ip = "127.0.0.1";
        int port = 1100;
	int speed = 750;

	CClient *cl = new CClient();
	cl->Connect(ip, port);
	cl->Talking(speed);

	delete cl;
	return 0;
}