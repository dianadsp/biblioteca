 /* Client code in C */
 
  #include <sys/types.h>
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>
  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  #include <unistd.h>
  #include <iostream>
  #include <string> 

  using namespace std;

  int chat(int SocketFD)
{
    char buffer[256];
    char buffer2[256];
    int n;
    do
    { 
        bzero(buffer,256);
        bzero(buffer2,256);
        cout << "Yo: ";
        cin.getline (buffer,256);

        n = write(SocketFD,buffer,256);
        

        n = read(SocketFD,buffer2,256);
        printf("El: [%s]\n",buffer2);
    }while (strncmp("END",buffer,3)!=0);

    shutdown(SocketFD, SHUT_RDWR);
 
    close(SocketFD);
}
 
  int main(void)
  {
    struct sockaddr_in stSockAddr;
    int Res;
    int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
 
    if (-1 == SocketFD)
    {
      perror("cannot create socket");
      exit(EXIT_FAILURE);
      return 0;
    }
 
    memset(&stSockAddr, 0, sizeof(struct sockaddr_in));
 
    stSockAddr.sin_family = AF_INET;
    stSockAddr.sin_port = htons(1100);
    Res = inet_pton(AF_INET, "192.168.8.101", &stSockAddr.sin_addr);
 
    if (0 > Res)
    {
      perror("error: first parameter is not a valid address family");
      close(SocketFD);
      exit(EXIT_FAILURE);
      return 0;
    }
    else if (0 == Res)
    {
      perror("char string (second parameter does not contain valid ipaddress");
      close(SocketFD);
      exit(EXIT_FAILURE);
      return 0;
    }
 
    if (-1 == connect(SocketFD, (const struct sockaddr *)&stSockAddr, sizeof(struct sockaddr_in)))  //conectarme con el servidor
    {
      perror("connect failed");
      close(SocketFD);
      exit(EXIT_FAILURE);
      return 0;
    }

    chat(SocketFD); //cierra el recurso 
    return 0;
  }
