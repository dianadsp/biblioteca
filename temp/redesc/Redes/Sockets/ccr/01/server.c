  /* Server code in C */

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
    struct sockaddr_in stSockAddr;
    int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    char buffer[256];
    int n;

    if(-1 == SocketFD)
    {
      perror("can not create socket");
      exit(EXIT_FAILURE);
    }

    memset(&stSockAddr, 0, sizeof(struct sockaddr_in));

    stSockAddr.sin_family = AF_INET;
    stSockAddr.sin_port = htons(1100);
    stSockAddr.sin_addr.s_addr = INADDR_ANY;

    //debemos linkear el stSockAddr con el socket

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

    for(;;)
    {
      int ClientFD = accept(SocketFD, NULL, NULL);

      if(0 > ClientFD )
      {
        perror("error accept failed");
        close(SocketFD);
        exit(EXIT_FAILURE);
      }


     bzero(buffer,256);   ///clean up the  buffer
     n = read(ClientFD ,buffer,255);

     /**
       n = 0 // no hay nada por recibir
     */

     if (n < 0) perror("ERROR reading from socket");
     printf("Here is the message: [%s]\n",buffer);
     n = write(ClientFD,"I got your message",18);
     if (n < 0) perror("ERROR writing to socket");

     /* perform read write operations ... */

      shutdown(ClientFD , SHUT_RDWR);

      close(ClientFD);
    }

    close(SocketFD);
    return 0;
  }
