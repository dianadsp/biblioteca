 /* Client code in C */
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
#include <vector>
#include <initializer_list>
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

////////////////////////////VARS/////////////////////////////////////

int isActiveConection;

////////////////////////////USER INPUT///////////////////////////////////

void inputServerData(char** hisPeerIP, int* hisPeerPort)
{
    *hisPeerIP = new char[100];
    printf("A cual server me conecto?\n");
    printf("IP: "); 
    scanf("%s", *hisPeerIP); 
    printf("Puerto: ");
    scanf("%d", hisPeerPort);
    cin.ignore(1, '\n');
}

///////////////////////////PARSER CMD/////////////////////////////////

string parseCmd(string cmd)
{
    string pack;
    vector<string> token = tokenizer(cmd);
    if(token.empty())
        pack = "error parsing";
    else if(token[0] == "nickname")
        pack = join({"1", len(token[1],2) ,token[1]});
    else if(token[0] == "list")
        pack = join({"2"});       
    else if(token[0] == "msg")
    {
        string msg = parserGetMsg(cmd);       
        pack = join({"3", len(token[1],2), token[1], len(msg,3), msg});
    }
    else if(token[0] == "exit")
        pack = join({"8"});
    else
        pack = "error parsing";
    return pack;
}

////////////////////////////////PACKS & REQUEST/RESPONSE/////////////////////////////////

string request2response(string pack)
{
    string response;
    switch(pack[0])
    {
        case '7': // msg
        {
	    response = join({"5"});
            break;
        }
        default:
            response = errorMsg("Paquete no valido de server"); // response
    }
    return response;
}

string response2string(string pack)
{
    string str;
    int idx = 2;
    switch(pack[0])
    {
        case '4': // Error
        {
            string errmsg = parserGetField(pack, idx, 3);
            str = errmsg; 
            break;
        }
        case '5': // OK
            str = "OK"; 
            break;
        case '6': // list
        {
            string listmsg = parserGetField(pack, idx, 3);
            str = listmsg; 
            break;
        }
        default:
            str = errorMsg("Comando no reconocido del server"); 
    }
    return str;
}


///////////////////////////////////SEND/RECEIVE PACKS/////////////////////////////////
//// SEND
void sendPackagesToServer(int sockFD)
{
    do
    {
        string cmd, pack;
        printf(">>");
        getline(cin, cmd);
        pack = parseCmd(cmd);
	if(pack == "8") // quit
            isActiveConection = false;
        if(pack == "error parsing")
            printf("Comando no valido\n");
	else
	{	
            // printf("Debug: envio [%s]\n", pack.c_str()); 
            ___write(sockFD, pack.c_str(), pack.size());
	}
    }while(isActiveConection);
}

//// RECEIVE

bool isRequest(string pack)
{
    return pack[0] == '7'; // msg
}

void printfClean(string reqres) // Evita que la salida consola descuadre con el >> del sendPackages()
{
     printf("\b\b%s\n>>", reqres.c_str()); // Elimino los '>>', imprimo, agrego otros '>>' 
     fflush(stdout);
}

void printIsRequestIsMessage(string pack)
{
    if(pack[0] != '7') return;
    char buf[280]; 
    bzero(buf, 280);
    int idx = 2;

    string nickFrom = parserGetField(pack, idx, 2);
    string msg = parserGetField(pack, idx, 3);
    sprintf(buf, "%s: %s", nickFrom.c_str(), msg.c_str());
    printfClean(string(buf));
}

void receivePackagesFromServer(int sockFD)
{
    do{ 
        string pack(1000, '\0');
        ___read(sockFD, (char*)pack.c_str(), pack.size());
        // printf("Debug: recibo [%s]\n", pack.c_str());

        string request, response; // Los packetes del server o son de tipo request, o response
        if(isRequest(pack))
        {
            printIsRequestIsMessage(pack);
            pack = request2response(pack);
            ___write(sockFD, pack.c_str(), pack.size());  // envio al server su respuesta
        }
        else
        {
            response = response2string(pack);
	    printfClean(response);
        }
    }while(isActiveConection);
}

void client_Thread()
{ 
    try
    {
        struct sockaddr_in stSockAddr;
        int sockFD, hisPeerPort;
        char* hisPeerIP; 
         // Crear socket
        sockFD = ___socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
        
        // Conectar a peer server
        /// ingresar datos usuario
        inputServerData(&hisPeerIP, &hisPeerPort);
        /// setear variables(IP, PORT)
        memset(&stSockAddr, 0, sizeof(struct sockaddr_in));
        stSockAddr.sin_family = AF_INET;
        stSockAddr.sin_port = htons(hisPeerPort);
        ___inet_pton(AF_INET, hisPeerIP, &stSockAddr.sin_addr);
    
        /// establecer conexion a server
        ___connect(sockFD, (struct sockaddr *)&stSockAddr, sizeof(struct sockaddr_in));
        printf("OK, conectado a server\n");
    
        // Enviar y Recibir paquetes
        thread t1(sendPackagesToServer, sockFD);
        thread t2(receivePackagesFromServer, sockFD);
        t1.join();
        t2.join();
        
        // Cerrar conexion 
        shutdown(sockFD, SHUT_RDWR);
        close(sockFD);
    }
    catch(string msg_error)
    {
        perror(msg_error.c_str());
        exit(EXIT_FAILURE);
    }
}

//////////////////////////////MAIN////////////////////////////////
// hostname -I
// Esta basado en modelo request-response
int main(int argc, const char** argv)
{
    while(true) // Al cerrar conexion puedo volver a abrir otra
    {
        isActiveConection = true;
        client_Thread();
    }
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


