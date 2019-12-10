#ifndef _SERVER_H_
#define _SERVER_H_

#include <iostream>
#include <vector>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <thread>
#include "worm.h"
#include "lib.h"

using namespace std;

class CData{
public:
	std::string m_name;
	int m_id;
	int m_sock;
	char m_avatar;

	CData();
	~CData(){	};

	void SetName(std::string);
	void SetId(int);
	void SetAvatar(char);
};

class CArg{
public:
	CData *m_client;
	TWorm *m_worm;
	CArg(){
		this->m_client = new CData();
		this->m_worm = new TWorm();
	}

	~CArg(){
		delete this->m_client;
		delete this->m_worm;	
	}
};

class CThread{
private:
	static pthread_mutex_t m_mutex;
public:
	pthread_t m_tid;

	CThread();
	~CThread();

	bool Create(void *, void *);
	bool Join();

	static bool InitMutex();
	static bool LockMutex(std::string);
	static bool UnlockMutex(std::string);
};

class CServer{
private:
	static vector<CData> m_clients;	
	static vector<TWorm> m_worms;
	static vector<pair<int, int> > food;

	int m_serverSock, m_clientSock;
	struct sockaddr_in m_serverAddr, m_clientAddr;
	static unsigned m_rsize;
	static unsigned m_csize;
	static char **m_table;
	char buffer[256];

	static int  FindClientIdx(char);
	static void SendToAll(std::string);
	static int  FindClientIdx(CData *);
	static bool IsAvatarUsed(char);
	static void UpdateTable(int);
	static void ClearMatrix();	
	static int  Find_Fruit(int, int);
public:
	CServer();
	~CServer();

	void Connect(int);
	void Listening();
	void CreateMatrix(unsigned, unsigned);
	void GenerateFood(int);
	static void * HandleClient(void *);
};

//------------------------------- DATA -----------------------------
CData::CData(){
	this->m_name = "";
}

void CData::SetName(std::string _name){
	this->m_name = _name;
}

void CData::SetId(int _id){
	this->m_id = _id;
}

void CData::SetAvatar(char _avatar){
	this->m_avatar = _avatar;
}

//-------------------------------- THREAD ---------------------------
pthread_mutex_t CThread::m_mutex;
CThread::CThread(){

}

bool CThread::Create(void *callback, void * arg){
	pthread_create(&this->m_tid, NULL, (void *(*)(void *))callback, arg);
	return true;
}

bool CThread::Join(){
	pthread_join(this->m_tid, NULL);
	return true;
}

bool CThread::InitMutex(){
	if(pthread_mutex_init(&CThread::m_mutex, NULL) < 0)
		return false;

	return true;
}

bool CThread::LockMutex(std::string identifier){
	if(pthread_mutex_lock(&CThread::m_mutex) == 0)
		return true;

	return false;
}

bool CThread::UnlockMutex(std::string identifier){
	if(pthread_mutex_unlock(&CThread::m_mutex) == 0)
		return true;

	return false;
}

//-------------------------------- SERVER ---------------------------

#endif