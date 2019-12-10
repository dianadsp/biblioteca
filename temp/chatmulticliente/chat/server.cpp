// Oscar Daniel Ramos Ramirez
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <thread>
#include <signal.h>
#include "stringhelper.h"

using namespace std;

//////////////////////////////AUX/////////////////////////////
// Agrega una capa de seguridad a cada funcion

int ___socket(int domain, int type, int protocol);
int ___inet_pton(int af, const char* src, void* dst);
void ___connect(int sockFD, struct sockaddr* serv_addr, int addrlen);
void ___bind(int sockFD, struct sockaddr* my_addr, int addrlen);
void ___listen(int sockFD, int backlog);
int ___accept(int sockFD, struct sockaddr* addr, socklen_t* addrlen);
void ___read(int fd, char buf[], int len);
void ___write(int fd, const char* msg, int len);

////////////////////////////THREADS///////////////////////////////////
class Client;
vector<Client*> clients;
// Permite clientes con mismo nick

class Client
{
private:
    int connectFD;
    string nickname = "";
    string lastResponse; // Para msg
    bool isActive = true;

public:
    Client(int connectFD): connectFD(connectFD)
    {
        clients.push_back(this);
    }

    // printf("debug: recibi [%s] de \"%s\"\n", pack.c_str(), this->nickname.c_str());
    // printf("debug: envio [%s] a \"%s\"\n", pack.c_str(), this->nickname.c_str());
    void receivePackages()
    {
        do
        {
            string pack(1000, '\0');
            ___read(connectFD, (char*)pack.c_str(), 1000);

	    if(isRequest(pack))
	    {
                pack = request2response(pack);
                ___write(connectFD, pack.c_str(), pack.size());
	    }
	    else
	    {
                lastResponse = response2string(pack);
	    }
        }while(this->isActive);

        // Cerrar conexion
        shutdown(connectFD, SHUT_RDWR);
        close(connectFD);
    }

    string request2response(string pack)
    {
        string response;
        int idx = 2;
        switch(pack[0])
        {
        case '1': // nick
        {
            string nick = parserGetField(pack, idx, 2);
            this->nickname = nick;
            response = join({"5"}); // OK
            break;
        }
        case '2': // list
        {
            vector<string> list;
            for(Client* c: clients) if(!c->nickname.empty())
                list.push_back(c->nickname);
            string listmsg = split(list, ", "); // "usr1, usr2, ..."
            response = join({"6", len(listmsg, 3), listmsg});
            break;
        }
        case '3': // msg
        {
            string tocli, msg;
            tocli = parserGetField(pack, idx, 2);
            msg = parserGetField(pack, idx, 3);
            for(Client* c: clients) if(c->nickname == tocli)
            {
		sendMsgToDestinataryClient(c, msg);
		if(destinatarySendedOk(c))
                    response = join({"5"});
                else
                    response = errorMsg("Error en cliente receptor");
            }
            if(response.empty())
               response = errorMsg("Cliente no encontrado");
            break;
        }
        
        case '8': // exit
            for(int i=0; i<clients.size(); ++i) if(clients[i] == this)
                clients.erase(clients.begin() + i);
            this->isActive = false;
            response = join({"5"}); // ok
            break;
        default:
            response = errorMsg("Comando no identificado");
        }
        if(this->nickname.empty() && pack[0] != '8')
            response = errorMsg("Necesitas un nickname!!"); 
        return response;
    }

    string response2string(string pack)
    {
	string str;
        switch(pack[0])
	{
        case '4': // error
            str = errorMsg("Error en cliente receptor");
	    break;
        case '5': // OK
            str = join({"5"});
            break;
	}
	return str;
    }

    void sendMsgToDestinataryClient(Client* c, string msg)
    {
        string pack;
        pack = join({"7", len(this->nickname, 2), this->nickname, len(msg, 3), msg});
        // printf("Enviando a destinatario [%s]\n", pack.c_str());
        ___write(c->connectFD, pack.c_str(), pack.size()); 
    }

    bool destinatarySendedOk(Client* c)
    {
        sleep(1); // Lo esperamos solo 1 segundo
        return c->lastResponse == join({"5"}); //OK
    }

    bool isRequest(string pack)
    {
        return pack[0] == '1' || pack[0] == '2' || pack[0] == '3' || pack[0] == '8';
    }
};

int server_Thread(int serverPort)
{
    try
    {
        // Crear socket
        int sockFD;
        struct sockaddr_in stSockAddr;  
        sockFD = ___socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
         
        // Crear peer server
        /// seteando variables servidor(PORT, IP AUTOMATICO)
        memset(&stSockAddr, 0, sizeof(struct sockaddr_in));
        stSockAddr.sin_family = AF_INET;
        stSockAddr.sin_port = htons(serverPort);
        stSockAddr.sin_addr.s_addr = INADDR_ANY; 

        ___bind(sockFD, (struct sockaddr*)&stSockAddr, sizeof(struct sockaddr_in));
        ___listen(sockFD, 10);
        printf("Servidor abierto\n");

        // Aceptar conexiones entrantes
        while(true)
        {
            int connectFD;
            connectFD = ___accept(sockFD, NULL, NULL);
            printf("cliente conexion recibida\n");
            Client* c = new Client(connectFD);
            thread th(&Client::receivePackages, c);
            th.detach();
        }
        // Cierro el servidor
        close(sockFD);
    }
    catch(string msg_error)
    {
        perror(msg_error.c_str()); // perror da mas detalle que printf 
        exit(EXIT_FAILURE);
    }
}
//////////////////////////////SIGNAL//////////////////////////////

void sighandler(int signum)
{
    exit(0); // Mas seguro segun vi
}

//////////////////////////////MAIN////////////////////////////////
// >>hostname -I: saca mi IP, en consola
int main(int argc, const char** argv)
{
    signal(SIGKILL, sighandler);
    if(argc != 2)
    {
        printf("Modelo : ./server.exe <Puerto del server>\n");
        printf("Ejemplo: ./server.exe 1480\n");
        exit(EXIT_FAILURE);
    }
    int serverPort = atoi(argv[1]);
    server_Thread(serverPort);
    return 0;
}




//////////////////////////////AUX/////////////////////////////
// Agrega una capa de seguridad a cada funcion
// No es necesario leerse todo esto, solo sirve para try-catch
int ___socket(int domain, int type, int protocol)
{
    int sockFD = socket(domain, type, protocol);
    if (-1 == sockFD)
      throw string("cannot create socket");
    return sockFD;
}

int ___inet_pton(int af, const char* src, void* dst)
{
    int res = inet_pton(af, src, dst);
    if (0 > res)
      throw string("error: first parameter is not a valid address family");
    else if (0 == res)
      throw string("char string (second parameter does not contain valid ipaddress");
    return res;
}

void ___connect(int sockFD, struct sockaddr* serv_addr, int addrlen)
{
    int res = connect(sockFD, serv_addr, addrlen);
    if(res == -1)
      throw string("connect failed");
}

void ___bind(int sockFD, struct sockaddr* my_addr, int addrlen)
{
    int res = bind(sockFD, my_addr, addrlen);
    if(res == -1)
      throw string("error bind failed");
}

void ___listen(int sockFD, int backlog)
{
    int res = listen(sockFD, backlog);
    if(res == -1)
      throw string("error listen failed");
}

int ___accept(int sockFD, struct sockaddr* addr, socklen_t* addrlen)
{
    int connectFD = accept(sockFD, addr, addrlen);
    if(connectFD < 0)
      throw string("error accept failed");
    return connectFD;
}

void ___read(int fd, char buf[], int len)
{
    int n = read(fd, buf, len);
    if(n < 0) 
      throw string("ERROR reading from socket");
}   

void ___write(int fd, const char* msg, int len)
{
    int n = write(fd, msg, len);
    if(n < 0)
      throw string("ERROR writing to socket");
}

