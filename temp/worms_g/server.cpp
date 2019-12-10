#include <iostream>
#include "server.h"

// build g++ -std=c++11 -pthread server.cpp -o server.out
using namespace std;
int number_food = 30;

CServer::CServer(){
	CThread::InitMutex();
	m_serverSock = socket(AF_INET, SOCK_STREAM, 0);	
}

void CServer::CreateMatrix(unsigned _r, unsigned _c){
	this->m_rsize = _r;
	this->m_csize = _c;

	this->m_table = new char*[m_rsize];

	unsigned i, j;

	for(i=0; i<m_rsize; i++){
		this->m_table[i] = new char[m_csize];
		for(j=0; j<m_csize; j++){
			this->m_table[i][j] = ' ';
		}
	}
	GenerateFood(number_food);
}

//create matrix of game
void CServer::ClearMatrix(){
	unsigned i, j;

	for(i=1; i<m_rsize-1; i++){
		for(j=1; j<m_csize-1; j++){
			m_table[i][j] = ' ';
		}
	}

	for(j=0; j<m_csize; j++){
		m_table[0][j] = ' ';
		m_table[m_rsize-1][j] = ' ';
	}

	for(i=0; i<m_rsize; i++){
		m_table[i][0] = ' ';
		m_table[i][m_csize-1] = ' ';
	}
}

void CServer::Connect(int _port){
	int reuse = 1;	
    memset(&m_serverAddr, 0, sizeof(sockaddr_in));

    m_serverAddr.sin_family = AF_INET;
    m_serverAddr.sin_addr.s_addr = INADDR_ANY;
    m_serverAddr.sin_port = htons(_port);

    //Avoid bind error if the socket was not close()
    setsockopt(m_serverSock,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(int));

    if(bind(m_serverSock, (struct sockaddr *) &m_serverAddr, sizeof(sockaddr_in)) < 0)
        perror("error: failed to bind");

    listen(m_serverSock, 5);
    cout << "Server created->\n";
}

void CServer::GenerateFood(int _t){
	int tr, tc;

	srand(time(NULL));
	for(unsigned i=0; i<_t; i++){		
		tr = 1+(rand()%(m_rsize-2));
		tc = 1+(rand()%(m_csize-2));

		food.push_back(std::make_pair(tr,tc));
	}
}

void CServer::Listening(){
	CArg    *arg;
	CThread *thr;

	socklen_t cli_size = sizeof(sockaddr_in);

	for(;;){
		arg = new CArg();
		thr = new CThread();

		// block
		arg->m_client->m_sock = accept(m_serverSock, (struct sockaddr *) &m_clientAddr, &cli_size);

	    if(arg->m_client->m_sock == -1)
	        perror("error: not accept");
	    else{
			thread t1(HandleClient, arg);
			t1.join();
		}
	        // thr->Create((void *) CServer::HandleClient, arg);
	}
}

int CServer::Find_Fruit(int _r, int _c){
	for(unsigned i=0; i<food.size(); i++){
		if((food[i].first == _r) and (food[i].second == _c)){
			return (int)i;
		}
	}

	return -1;
}

void CServer::UpdateTable(int _idx){
	unsigned i, j;

	ClearMatrix();

	for(i=0; i<food.size(); i++)
		m_table[food[i].first][food[i].second] = '0';

	std::pair<int, int> tmp;
	for(i=0; i<m_worms.size(); i++){
		tmp = m_worms[i].m_head;
		
		for(auto pos:m_worms[i].m_body){
			m_table[pos.first][pos.second] = m_clients[i].m_avatar;
		}
		// m_table[m_worms[i].m_head.first][m_worms[i].m_head.second] = 'O';
	}

	int itm;
	int s;

	tmp = m_worms[_idx].m_head;
	itm = Find_Fruit(tmp.first, tmp.second);

	if(itm >= 0){
		CServer::food.erase(CServer::food.begin()+itm);
		m_worms[_idx].Grow(tmp.first, tmp.second);
	}

	for(j=0; j<m_worms.size(); j++){
		if(_idx!=j){
			if(m_worms[j].IsThere(tmp.first, tmp.second)){					
				s = m_worms[j].RemoveBody();					
				m_worms[_idx].AddQueue(s);
				// std::cout << m_clients[_idx].m_avatar << " eats " << s << " tails of " << m_clients[j].m_avatar << "\n";
				break;
			}
		}
	}
}

bool CServer::IsAvatarUsed(char _a){
	for(unsigned i=0; i<m_clients.size(); i++)
		if(m_clients[i].m_avatar == _a)
			return true;

	return false;
}

void *CServer::HandleClient(void *_args){
	CArg  *arg = (CArg  *)_args;
	
	CData *cli = arg->m_client;
	TWorm *wor = arg->m_worm;

	char buffer[256-25];
	string text = "";

	int idx, n;

	CThread::LockMutex(cli->m_name);

		cli->SetId(CServer::m_clients.size());
		cli->SetName("[client "+std::to_string(cli->m_id)+"]");

		bool avatar = true;
		while(avatar){
			memset(buffer, 0, sizeof(buffer));
			n = recv(cli->m_sock, buffer, sizeof(buffer), 0);
			
			if(n > 0){
				if(!CServer::IsAvatarUsed(buffer[0])){
					cli->SetAvatar(buffer[0]);					
					avatar = false;

					text = "1";
					send(cli->m_sock, text.c_str(), text.size(), 0);
				}
				else{
					text = "0";
					send(cli->m_sock, text.c_str(), text.size(), 0);
				}
			}
		}

		srand(time(NULL));
		unsigned tr = 1+(rand()%(m_rsize-2));
		unsigned tc = 1+(rand()%(m_csize-2));

		wor->SetHead(tr, tc);

		cout << cli->m_name << " connected\tid: " << cli->m_id << "\tavatar: " << cli->m_avatar << "\n";
		CServer::m_clients.push_back(*cli);
		CServer::m_worms.push_back(*wor);		
	CThread::UnlockMutex(cli->m_name);	
	
	while(true){
		memset(buffer, 0, sizeof(buffer));
		n = recv(cli->m_sock, buffer, sizeof(buffer), 0);

		if(n == 0){
			std::cout << cli->m_name << " disconneted\n";
			close(cli->m_sock);

			CThread::LockMutex(cli->m_name);
				idx = CServer::FindClientIdx(cli);
				CServer::m_clients.erase(CServer::m_clients.begin()+idx);
				CServer::m_worms.erase(CServer::m_worms.begin()+idx);
			CThread::UnlockMutex(cli->m_name);
			break;
		}
		else if(n < 0){
			perror("error: receiving text");
		}
		else{
			idx = CServer::FindClientIdx(cli);
			m_worms[idx].Move(buffer[0], m_rsize, m_csize);
			CServer::UpdateTable(idx);
			text = table_to_str(m_table, m_rsize, m_csize);
			CServer::SendToAll(text);
		}
	}

	return NULL;
}

void CServer::SendToAll(std::string text){
	CThread::LockMutex("'Send'");
		// std::cout << "\ntext sending: " << text << "\n";
		for(unsigned i=0; i<m_clients.size(); i++)
			send(CServer::m_clients[i].m_sock, text.c_str(), text.size(), 0);
	CThread::UnlockMutex("'Send'");
}

int CServer::FindClientIdx(CData *_cli){
	for(unsigned i=0; i<m_clients.size(); i++)
		if(CServer::m_clients[i].m_id == _cli->m_id)
			return i;

	return 0;
}

int CServer::FindClientIdx(char _a){
	for(unsigned i=0; i<m_clients.size(); i++)
		if(CServer::m_clients[i].m_avatar == _a)
			return i;

	return -1;
}

CServer::~CServer(){
	m_clients.clear();
}

std::vector<CData> CServer::m_clients;
std::vector<TWorm> CServer::m_worms;
std::vector<std::pair<int, int> > CServer::food;
char** CServer::m_table;

unsigned CServer::m_rsize;
unsigned CServer::m_csize;

int main(int argc, char const *argv[]){
	int port = 1100;
	unsigned rsize = 30;
	unsigned csize = 60;

	CServer *s = new CServer();

	s->Connect(port);
	s->CreateMatrix(rsize, csize);

	s->Listening();		// loop

	delete s;
	return 0;
}