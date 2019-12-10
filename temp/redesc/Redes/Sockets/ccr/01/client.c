 /* Client code in C */

  #include <sys/types.h>
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>
  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  #include <unistd.h>

  int main(void)
  {
    struct sockaddr_in stSockAddr;  //define caracteristicas de comunicacion
    int Res;
    int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP); //fd file descriptor, pf inet socket para internet
    int n;
    char buffer[1000];

    if (-1 == SocketFD)
    {
      perror("cannot create socket");
      exit(EXIT_FAILURE);
    }

    memset(&stSockAddr, 0, sizeof(struct sockaddr_in));  //limpia stSockAddr

    stSockAddr.sin_family = AF_INET;
    stSockAddr.sin_port = htons(1100); //45001
    Res = inet_pton(AF_INET, "192.168.1.33", &stSockAddr.sin_addr);  //identifica el puerto del servidor

    //validdation of assining
    if (0 > Res)
    {
      perror("error: first parameter is not a valid address family");
      close(SocketFD);
      exit(EXIT_FAILURE);
    }
    else if (0 == Res)
    {
      perror("char string (second parameter does not contain valid ipaddress");
      close(SocketFD);
      exit(EXIT_FAILURE);
    }
    //end of validation of assining

    if (-1 == connect(SocketFD, (const struct sockaddr *)&stSockAddr, sizeof(struct sockaddr_in)))
    {
      perror("connect failed");
      close(SocketFD);
      exit(EXIT_FAILURE);
    }
    n = write(SocketFD,"Hi, this is Julio.",18); //18 bytes
    /* perform read write operations ... */

     n = read(SocketFD,buffer, 100);


    shutdown(SocketFD, SHUT_RDWR);

    close(SocketFD);
    return 0;
  }
