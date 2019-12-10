  /* Server code in C */
 
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
#include <chrono>         
#include <string>
#include <sstream>
#include <cstdlib>
#include <mutex>
#include <condition_variable>
#include <map>

using namespace std;

  void readC(int SocketP)
  {
    char buffer[256];
    int n=0;

    bzero(buffer,256);
    n = (SocketP, buffer, 256);

    if(n < 0)
    perror("Error de lectura del socket");
    
  }

  void writeC(int SocketP)
  {
    char msg[256];
    int n=0;
    cin.getline(msg,256);
    n = write(SocketP,msg,256);

    if(n < 0)
      perror("Error escribiendo a socket");

  }

  void clientes(int SocketP, int i)
  {
    char buffer[256];
    int n=0;
    char msg[256];

    do{

        thread rd(readC, SocketP);

        printf("Client %d: %s\n", i, buffer);

        cout << "Cliente " <<i<<" dice: "<<buffer << endl;
        
        thread wt(writeC, SocketP);
        cout << "Escribe servidor:  ";

        rd.join();
        wt.join();
 
    }while (strncmp("FIN",buffer,3)!=0);

    shutdown(SocketP, SHUT_RDWR);
    close(SocketP);
  }

  int main(void)
  {
    struct sockaddr_in stSockAddr, client_addr;
    int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    char buffer[256];
    char msg[256];
    int n;
 
    if(-1 == SocketFD)
    {
      perror("can not create socket");
      exit(EXIT_FAILURE);
    }
 
    memset(&stSockAddr, 0, sizeof(struct sockaddr_in));
 
    stSockAddr.sin_family = AF_INET;
    stSockAddr.sin_port = htons(1100);
    stSockAddr.sin_addr.s_addr = INADDR_ANY; // aqui indica una sola tarjeta de red
 
    if(-1 == bind(SocketFD,(const struct sockaddr *)&stSockAddr, sizeof(struct sockaddr_in)))
    {
      perror("error bind failed");
      close(SocketFD);
      exit(EXIT_FAILURE);
    }
 
    if(-1 == listen(SocketFD, 10))
    {
      perror("error listen failed");
      close(SocketFD);
      exit(EXIT_FAILURE);
    }

    socklen_t len;
    int i=1;
 
    for(;;)
    {
      len = sizeof(client_addr);

      int ConnectFD = accept(SocketFD, (struct sockaddr *)&client_addr, &len);  //cuando el cliente se conecta se cnecta con el accept y el socket descriptor que me va a indicar la conexion, socket del servidor
 
      if(0 > ConnectFD)
      {
          perror("error accept failed");
          close(SocketFD);
          exit(EXIT_FAILURE);
      }else{
          printf("Connected client %d\n",i);
          thread(clientes,ConnectFD,i).detach();
          i++;
      } 
      
    }
 
    close(SocketFD); //se cierra el socket del servidor
    return 0;
  }
