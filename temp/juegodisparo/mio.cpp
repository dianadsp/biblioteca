#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <vector>
#include <curses.h>
#include <unistd.h>
#include <ncurses.h>     // std::cout
#include <thread>         // std::thread, std::this_thread::sleep_for
#include <chrono> 
//compila asi g++ mio.cpp -lncurses -o a

using namespace std;


typedef struct _win_border_struct 
{
        chtype  ls, rs, ts, bs, tl, tr, bl, br;
}
WIN_BORDER;

typedef struct _WIN_struct 
{
        int height, width;
        WIN_BORDER border;
}
WIN;


vector< vector<char> > matrix;
int dimensionL=20;
int dimensionA=75;
int dispara=0;
vector <char> naves;
int direccion=0;
int tecla=0;
char space= ' ';


int mod (int a,int b )
{
    if(a<0)
    {
        return a+b;
    }
    return a%b;
}


//llena la matriz
void inicia(int dimL, int dimA)
{

     vector<char> linea;
     matrix.clear();
     for(int i =0; i<dimA; i++)
     {
        linea.push_back(space);
     }
     for(int i =0; i<dimL; i++)
     {
        matrix.push_back(linea);
     }
}


void imprimir ()
{   
    clear();
    for(int i =0; i<dimensionL; i++)
     {  
        for(int j =0; j<dimensionA; j++)
         {
            printw ("%c", matrix[i][j]);
         }
         printw("\n");
     }
     cbreak();
     refresh();   
}

void balas (int a , int b, int direccion,char buffer1[2])
{   //inicia(dimension);
    char bala= '*';
    //cout<<"HERE"<<direccion<<endl;
    int dir = direccion-48;
    switch (dir) 
    {
        case 8://Arriba
          for(int i=0; i<a;i++)///a es y 
            {   
               matrix[i][b]=bala;
            }
          break;
        case 2:///abajo
          for(int i=a+2; i<dimensionL ;i++)
          {
                 matrix[i][b]=bala;
          }
          break;
        case 6: //derecha
          for(int i=b+2; i<dimensionA ;i++)
          {
                matrix[a][i]=bala;
          }
          break;
        case 4: ///izquierda
          for(int i=0; i<b ;i++)
          {
                 matrix[a][i]=bala;
          }
          break;
        default:
          break;        
    }

 
}



int a=0;
int b=0;    
vector< pair <int, pair<int, int> > > usuarios;

int teclas ()
{

    tecla = getch ();
    if(tecla==115)
    {
        a=(a+1)%(dimensionL-1);//S
        direccion=5;
    }
    if(tecla==97 )
    {   b=(b-1)%(dimensionA-1);//A
        direccion=4;
    }
    if(tecla==119)
    {   
        a=(a-1)%(dimensionL-1);//W
        direccion=8;
    }
    if(tecla==100)
    {
        b=(b+1)%(dimensionA-1);//D
        direccion=6;
    }
    if(tecla==56)dispara=8; //8
    //if(tecla==57)dispara=9; //9
    if(tecla==54)dispara=6; //6
    //if(tecla==51)dispara=3; //3
    if(tecla==50)dispara=2; //2
    //if(tecla==49)dispara=1; //1
    if(tecla==52)dispara=4; //4
    //if(tecla==55)dispara=7; //7
    if(a<0)a=dimensionL-2;
    if(b<0)b=dimensionA-2;
    //endwin ();
}




void cuadrado( int x, int y, char letra )
{   //inicia(dimension);
    matrix[x][y]=letra;
    matrix[x][y+1]=letra;
    //matrix[x][y+2]=letra;
   // matrix[x][y+4]='#';
    matrix[x+1][y]=letra;
    matrix[x+1][y+1]=letra;
    //matrix[x+1][y+2]=letra;
   // matrix[x+1][y+3]='#';
    //matrix[x+1][y+4]='#';
    //matrix[x+2][y]=letra;
    //matrix[x+2][y+1]=letra;
    //matrix[x+2][y+2]=letra;
   // matrix[x+2][y+2]='#';
   // matrix[x+2][y+3]='#';
    //matrix[x+3][y+2]='#';
    
    //teclas();
    //imprimir();
}


void main_no_main(char buffer[7])
{
    //cout<<"HEREL"<<endl;
    //teclas();
    char integer_string[32];

    
    if(dispara>0)buffer[0]='S';//strcat(buffer,"1");
    else         buffer[0]='M';//strcat(buffer,"0");

    if(a<=9) strcat(buffer,"0"); 
    sprintf(integer_string, "%d", a);  
    strcat(buffer,integer_string); 

    if(b<=9) strcat(buffer,"0"); 
    sprintf(integer_string, "%d", b);
    strcat(buffer,integer_string); 
    
    if(dispara>0)
    {
        sprintf(integer_string, "%d", dispara);
        strcat(buffer,integer_string); 
    }
    else
    {
        sprintf(integer_string, "%d", direccion);
        strcat(buffer,integer_string); 
    }
   
    dispara=0;

}
