#ifndef _SLAVE_H_
#define _SLAVE_H_

#include <string.h>
#include <iostream>

#include "database.h"
#include "protocol.h"
#include "utils.h"

using namespace std;

class CSlave{
    public:
        int bits;
        CInfo m_info; //port , ip
        CAddress m_conexion; //CAdress contiene socket y struct de socket (utils.h)
        CDatabase *m_db; //puntero a BD

        void escuchar();
        void inicializar();

        CSlave(int, string, string);
        ~CSlave();
};

CSlave::CSlave(int port, string ip, string dbname){
	this->bits = 64;
	this->m_info = {port, ip};
	this->m_conexion.socket = socket(AF_INET, SOCK_STREAM, 0);
    this->m_db = new CDatabase(dbname);
}


void CSlave::escuchar(){
    socklen_t qsize = sizeof(sockaddr_in);
    CAddress conexion;
    string comando, sql,sql_intermedias;//sql por niveles->sql_intermedias

    vector<string> nodos;
    vector<string> parse;
    vector<vector<string> > sresult;//matriz con el select del nodo en Q N/Q R| también funciona como temporal de Q R A 4-->PROFUNDIDAD
    vector<string> tresult;//resultados totales de todos los niveles
    CInfo info;
    CProtocol prot(bits);    

    while(true){
        conexion.socket = accept(m_conexion.socket, (struct sockaddr *) &conexion.direccion, &qsize);
        if(conexion.socket < 0)
            perror("Error on accept");
        else{           
            comando = prot.recibir(conexion.socket);
            cout << comando << "\n";
            switch(comando[0]){
                case 'I':{
                    parse = dividir_mensaje(comando.substr(1), "|");
	    			configurar_cliente(parse, info, sql);
                    if(m_db->execute(sql)){
                        conectar_enviar(info, "Create", "O", bits);
                    }
                    else{
                        conectar_enviar(info, "Create", "E", bits);
                    }
                    break;
                }                
                case 'D': {
                    parse = dividir_mensaje(comando.substr(1), "|");
                    configurar_cliente(parse, info, sql);
                    if(m_db->execute(sql)){
                        conectar_enviar(info, "Delete", "O", bits);
                    }
                    else{
                        conectar_enviar(info, "Delete", "E", bits);
                    }
                    break;
                }
                case 'U': {
                    parse = dividir_mensaje(comando.substr(1), "|");
                    configurar_cliente(parse, info, sql);
                    if(m_db->execute(sql)){
                        conectar_enviar(info, "Update", "O", bits);
                    }
                    else{
                        conectar_enviar(info, "Update", "E", bits);
                    }
                    break;
                }     
                case 'Q':{
                    parse = dividir_mensaje(comando.substr(1), "|");
                    configurar_cliente(parse, info, sql);
                    if(m_db->execute(sql, sresult)){
                        conectar_enviar(info, matriz_to_string(sresult, "|", "/"), "Q", bits);
                    }
                    else{
                        conectar_enviar(info, "Query", "E", bits);
                    }
                    break;
                }
                case 'R':{
                    parse = dividir_mensaje(comando.substr(1), "|");
                    obtener_hijos(parse, info, sql, nodos);
                    
                    tresult.clear();
                    for(unsigned i=0; i<nodos.size(); i++){
                        sql_intermedias = sql + nodos[i] + "';";

                        m_db->execute(sql_intermedias, sresult);//sresult se ponen sus hijos

                        if(sresult.size() > 1){
                            for(unsigned j=1; j<sresult.size(); j++){
                                tresult.push_back(nodos[i] + "/" + sresult[j][2]);//a->b/a->c --> Id Nodo1 NOdo2 y quiero ir a Nodo2
                            }
                        }
                    }
                    conectar_enviar(info, vector_to_string(tresult, "|") , "R", bits);
                    break;
                }

            }
        }
        shutdown(conexion.socket, SHUT_RDWR);
        close(conexion.socket);
    }

    shutdown(m_conexion.socket, SHUT_RDWR);
    close(m_conexion.socket);
}

void CSlave::inicializar(){
	int reuse = 1;
    memset(&m_conexion.direccion, 0, sizeof(sockaddr_in));

    m_conexion.direccion.sin_family = AF_INET;
    m_conexion.direccion.sin_addr.s_addr = INADDR_ANY;
    m_conexion.direccion.sin_port = htons(m_info.port);

    setsockopt(m_conexion.socket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int));

    int nr = bind(m_conexion.socket, (struct sockaddr *) &m_conexion.direccion, sizeof(sockaddr_in));
    if(nr < 0){
        perror("Failed to bind");
    }
    listen(m_conexion.socket, 10);
    
	escuchar();
}

CSlave::~CSlave(){
}
#endif