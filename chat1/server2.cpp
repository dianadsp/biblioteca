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

 int nbites =4;
using namespace std;

 /* void readC(int SocketP)
  {
    char buffer[256];
    int n=0;

    bzero(buffer,256);
    n = (SocketP, buffer, 256);

    if(n < 0)
    perror("Error de lectura del socket");
    
  }*/
  void readC(int SocketP,char *msg)//char buffer[]) //leer 
  { 
      char buffer[4];
      int n=0;
      n = read(SocketP, buffer, 4);
      bzero(msg,atoi(buffer));
      n = read(SocketP, msg, atoi(buffer));
    //cout<<"mensaje->:"<<buffer<<endl;


    /*int n=0;
    string tmp1;
    char bufferTmp[4];

   // bzero(buffer,256);

    n = read(SocketP,bufferTmp,4);
    cout<<"string bufferTmp->"<<bufferTmp<<endl;
    cout<<endl;

   string tmp(bufferTmp);

   tmp1=tmp.substr(0,4);//aqui esta el tamaño

   cout<<"tamano -> "<<tmp1<<endl;*/
   /*cout<<"tmp:"<<tmp<<endl;
   tmp=tmp.substr(4,tmp.size());
   cout<<"tmp:"<<tmp<<endl;

    strcpy(buffer,tmp.c_str());*/


    if(n < 0)
    perror("Error de lectura del socket");
    
  }

  void writeC(int SocketP)
  {
    int n;
    string st, aux = "";
     int dif = 0;
     cout<<">> :";
     getline(cin, st);
     aux = to_string(st.size());
     dif = 4 - int(aux.size());
     st = aux + st;
     for (int i = 0; i < dif; i++)
          {st = "0" + st;}
   // cout<<"envia "<< st<<endl;
    n = write(SocketP,st.c_str(),st.size());

    if(n < 0)
      perror("Error escribiendo a socket");
    /*char msg[256];
    int n=0;
    cin.getline(msg,256);
    n = write(SocketP,msg,256);

    if(n < 0)
      perror("Error escribiendo a socket");*/

  }

  void clientes(int SocketP, int i)
  {
    char buffer[256];// este tiene que ser dinamico;
    char *mensaje2;
    int n=0;
    char msg[256];

    do{
        bzero(buffer,256);
        thread rd(readC, SocketP,buffer);
        rd.join();
        cout<<endl;
        cout << "Cliente " <<i<<" dice: "<<buffer<< endl;
        
        thread wt(writeC, SocketP);
        cout << "Escribe servidor:  ";
        wt.join();
         //rd.join();
 
    }while (strncmp("FIN",buffer,3)!=0);

    shutdown(SocketP, SHUT_RDWR);
    close(SocketP);
  }

  //*********************************************************************

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
