 /* Client code in C */
 //COMPILAR g++ client.cpp -std=c++11  -pthread -lncurses -o c
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
 #include "mio.cpp"
  
 using namespace std;


 struct sockaddr_in stSockAddr;
 int Res;
 int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
 int n;


 void update_bala(char dir,int a, int b)
 {
  switch (dir) 
  {
      case '8'://Arriba
        for(int i=0; i<a;i++)///a es y
              matrix[i][b]=' ';
        break;
      case '2':///abajo
        for(int i=a+2; i<dimensionL ;i++)
          matrix[i][b]=' ';
        break;
      case '6': //derecha
        for(int i=b+2; i<dimensionA ;i++)
          matrix[a][i]=' ';
        break;
      case '4': ///izquierda
        for(int i=0; i<b ;i++)
          matrix[a][i]=' ';
         break;
      default:
        break;      
  }
 }


 void update(char dir, int x, int y, char dis)
 { 
         if(dir=='6') //derecha
         {
            matrix[x][mod(y-1,dimensionA)]=' ';
            matrix[(x+1)][mod(y-1,dimensionA)]=' ';
           if(y-1<0)
           {
             matrix[x][mod(y-2,dimensionA)]=' ';
             matrix[(x+1)][mod(y-2,dimensionA)]=' ';
           }
         }
         if(dir=='4') //izquierd
         {
             matrix[x][mod(y+2,dimensionA)]=' ';
             matrix[(x+1)][mod(y+2,dimensionA)]=' ';
           if(y+2>dimensionL-1)
           {
             matrix[x][mod(y+3,dimensionA)]=' ';
             matrix[(x+1)][mod(y+3,dimensionA)]=' ';
           }
   
         }
             if(dir=='8')  //esto es arriba
         {
            matrix[mod(x+2,dimensionL)][y]=' ';
            matrix[mod(x+2,dimensionL)][(y+1)]=' ';
           if(x+2>dimensionL-1)
           {
            matrix[mod(x+3,dimensionL)][y]=' ';
            matrix[mod(x+3,dimensionL)][(y+1)]=' ';
           }
         }
         if(dir=='5')  //8 es bajo
         {
            matrix[mod(x-1,dimensionL)][y]=' ';
            matrix[mod((x-1),(dimensionL))][(y+1)]=' ';
           if(x-1<0)
           {
            matrix[mod(x-2,dimensionL)][y]=' ';
            matrix[mod((x-2),(dimensionL))][(y+1)]=' ';
           }
         }
 }


 

 void writeS()
 {
     while(true)
     {
       char buffer[7];
       teclas();
       for (int i = 0; i < sizeof(buffer); i++)
       {
           buffer[i] = '\0';
       } 
       main_no_main(buffer);
       n = write(SocketFD, buffer, sizeof(buffer));
       
     }

 }


 void readS()
 {
    while(true)
    {
      char buffer[7];
      //cout<<"BufferREadS: "<<buffer<<endl;
      n = read(SocketFD,buffer,sizeof(buffer));
      if (n < 0) perror("ERROR reading from socket");

      string ac ;
      string ab ;
      ac += buffer[2];
      ac += buffer[3];
      ab += buffer[4];
      ab += buffer[5];
      //cout<<"ac: "<<ac<<" ab: "<<ab<<endl;
      int a = stoi(ac);                  //De char a entero
      int b = stoi(ab);
      //cout<<"a: "<<a<<" b: "<<b<<endl;
      

      cuadrado(a,b,'A'+ buffer[0]);
      update(buffer[6],a,b,buffer[1]);
    
      char buffer1[2];
      if(buffer[1]=='S')balas(a,b, buffer[6],buffer1);
        
    WIN win;
        WIN win2;
        int ch;
        initscr();                      /* Start curses mode            */
        start_color();                  /* Start the color functionality */
        cbreak();                       /* Line buffering disabled, Pass on
                                         * everty thing to me           */
        keypad(stdscr, TRUE);           /* I need that nifty F1         */
        noecho();
        init_pair(1, COLOR_CYAN, COLOR_BLACK);

 
      imprimir();
      update_bala(buffer[6],a,b);
     
    }
 }

 int main(void)
 {

   initscr ();  
   inicia(dimensionL, dimensionA);


   if (-1 == SocketFD)
   {
     perror("cannot create socket");
     exit(EXIT_FAILURE);
   }

   memset(&stSockAddr, 0, sizeof(struct sockaddr_in));

   stSockAddr.sin_family = AF_INET;
   stSockAddr.sin_port = htons(1200);
   Res = inet_pton(AF_INET, "192.168.1.52", &stSockAddr.sin_addr);
   Res = inet_pton(AF_INET, "192.168.164.37", &stSockAddr.sin_addr);

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

   if (-1 == connect(SocketFD, (const struct sockaddr *)&stSockAddr, sizeof(struct sockaddr_in)))
   {
     perror("connect failed");
     close(SocketFD);
     exit(EXIT_FAILURE);
   }


   //imprimir();
   thread t1 (readS); 
   thread t2 (writeS); 
   t1.join();
   t2.join();
     /*t1.join();
     t2.join();
     t3.join();*/

   shutdown(SocketFD, SHUT_RDWR); 
   close(SocketFD);
   endwin ();
   return 0;
 }
