#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_

#include <vector>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;

class CProtocol{
	public:
		int tam_buffer;
		int m_bit;

		vector<string> dividir_query(string);
		CProtocol(int);
		~CProtocol();

		void enviar(string, int, string);
		string recibir(int);
};

CProtocol::CProtocol(int tam){
	this->tam_buffer = tam;
	this->m_bit = to_string(tam_buffer).size();	
}

string anadir_ceros(int num, int tam){
	string tmp = to_string(num);
	for(int i=tmp.size(); i<tam; i++){
		tmp = '0'+tmp;
	}
	return tmp;
}

vector<string> CProtocol::dividir_query(string mensaje){
	int tam=tam_buffer-(m_bit + 1);//tamano de mensaje
	vector<string> paquetes;//vector de paquetes
	string msg;
	for(int i=0; i<mensaje.size(); i+=tam){
		msg = "";
		for(int j=0; j<tam and (i + j)<mensaje.size(); j++){
			msg += mensaje[i+j];
		}
		paquetes.push_back(msg);
	}
	return paquetes;
}

void CProtocol::enviar(string mensaje, int socket, string tipo_sql){
	string tmp = tipo_sql + mensaje;
	vector<string> messages = dividir_query(tmp);
	string msg;

	int tmps = messages.size();

	if(tmps > 1){
		for(int i=0; i<tmps-1; i++){
			msg = "0" + anadir_ceros(messages[i].size(), m_bit) + messages[i];
			send(socket, msg.c_str(), msg.size(), 0);
		}
	}
	msg =  "1" + anadir_ceros(messages[tmps-1].size(), m_bit) + messages[tmps-1];//le pongo 1 para saber que es el ultimo
	send(socket, msg.c_str(), msg.size(), 0);
}

string CProtocol::recibir(int socket){
	string text="";
	string tmp="";
	char buffer[tam_buffer];

	int n;
	while(true){
		memset(&buffer, 0, tam_buffer);
        n = recv(socket, buffer, tam_buffer, 0);

        if(n>(m_bit+1)){
        	if(buffer[0] == '0'){
        		text += string(buffer).substr(m_bit+1, tam_buffer - (m_bit + 1));
        	}
        	else if(buffer[0] == '1'){
        		tmp = string(buffer).substr(1,m_bit);
        		text += string(buffer).substr(m_bit+1, stoi(tmp));
        		break;
        	}
        }
	}

	return text;
}

CProtocol::~CProtocol(){
}

#endif