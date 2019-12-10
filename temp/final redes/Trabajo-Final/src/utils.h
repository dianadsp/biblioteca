#ifndef _UTILS_H_
#define _UTILS_H_

#include <boost/algorithm/string.hpp> 
#include <netinet/in.h>

#include <sys/stat.h>
#include <utility>
#include <fstream>

#include "protocol.h"

using namespace std;

struct CInfo{//informacion del slave
	int port;
	string ip;
};

struct CAddress{
	int socket;	//socket
    struct sockaddr_in direccion; //direccion de socket
};

string info_to_string(CInfo info){
	return to_string(info.port) + "|" + info.ip;
}

void conectar_enviar(CInfo &slave, string mensaje,string tipo_consulta, int bits){
    struct sockaddr_in sockAddr;
    int msock = socket(AF_INET, SOCK_STREAM, 0);
    if(msock < 0){
        perror("Can't Create Socket!");
        exit(0);
    }

    memset(&sockAddr, 0, sizeof(sockAddr));
    sockAddr.sin_family = AF_INET;
    sockAddr.sin_port = htons(slave.port);
 
    int nr = inet_pton(AF_INET, slave.ip.c_str(), &sockAddr.sin_addr);
 
    if(nr <= 0){
        perror("error: Not a valid address family");
        close(msock);
        exit(0);
    }

    nr = connect(msock, (const struct sockaddr *)&sockAddr, sizeof(struct sockaddr_in));
    if(nr >= 0){
        CProtocol prot(bits);
        prot.enviar(mensaje, msock, tipo_consulta);
    }

    shutdown(msock, SHUT_RDWR);
    close(msock);
}

void configurar_cliente(vector<string> &parse, CInfo &info, string &sql){
    if(parse.size() == 3){
        info.port=stoi(parse[0]);
        info.ip=parse[1];
        sql=parse[2];
    }
    else{
        cout<<"Colocar port, ip y consulta"<<endl;
    }
}

vector<string> dividir_mensaje(string mensaje, string _separator){
    vector<string> result; 
    boost::split(result, mensaje, boost::is_any_of(_separator));
    return result;
}

int calcular_hash(string nodo, int modulo){
    int hash = 0;
    for(char c: nodo){
        hash += c;
    }
    return hash%modulo;
}

string matriz_to_string(vector<vector<string> > &_mat, string _sr, string _sc){//matriz, separador de filas, separador de columnas
    string all = "";
    string sub = "";

    for(int i=0; i<_mat.size(); i++){
        sub = "";
        for(int j=0; j<_mat[i].size(); j++){
            sub += _sc + _mat[i][j];
        }
        
        all += _sr + sub.substr(1);
    }

    if(all == ""){
        all = _sr + "No se encontro";
    }

    return all.substr(1);
}

void string_to_matrix(string _mat, string _sr, string _sc, string _iden){
    vector<string> rows = dividir_mensaje(_mat, _sr);
    vector<string> cols;

    for(int i=0; i<rows.size(); i++){
        cols = dividir_mensaje(rows[i], _sc);
        cout << _iden;
        for(int j=0; j<cols.size(); j++){
            cout << cols[j] << " ";
        }
        cout << "\n";
    }
}

void obtener_hijos(vector<string> &parse, CInfo &info, string &sql, vector<string> &nodos){
    if(parse.size() > 3){
        info.port  = stoi(parse[0]);
        info.ip    = parse[1];
        sql        = parse[2];

        nodos.clear();
        for(unsigned i=3; i<parse.size(); i++){//obtengo hijos del nodo que consulto
            nodos.push_back(parse[i]);
        }
    }
    else{
        cout<<"Colocar port, ip y consulta"<<endl;
    }
}

string vector_to_string(vector<string> &_list, string _sr){
    string all = "";

    if(_list.size() > 0){
        for(unsigned i=0; i<_list.size(); i++){
            all += _sr + _list[i];
        }
    }
    else{
        return "Nada";
    }

    return all.substr(1);
}

vector<string> str_to_list(string _mat, string _sr){
    vector<string> rows = dividir_mensaje(_mat, _sr);
    if(rows[0] == "Nada"){
        rows.clear();
    }
    return rows;
}

void add_str_to_list(vector<pair<string, string> > &_result, vector<string> &_inter, string _mat, string _sr, string _sc){
    vector<string> rows = str_to_list(_mat, _sr);
    vector<string> cols;

    bool t;
    unsigned i,j;
    for(i=0; i<rows.size(); i++){
        cols = dividir_mensaje(rows[i], _sc);

        t = false;
        for(j=0; j<_result.size(); j++){
            if(cols[1] == _result[j].second){
                t = true;
                break;
            }
        }
        if(!t){
            _inter.push_back(cols[1]);
        }

        t = false;
        for(j=0; j<_result.size(); j++){
            if(( (cols[0] == _result[j].first) and (cols[1] == _result[j].second) ) or 
                 ( (cols[1] == _result[j].first) and (cols[0] == _result[j].second) )) {
                t = true;
                break;
            }
        }
        if(!t){
            _result.push_back({cols[0], cols[1]});
        }
    }
}

void str_to_list(vector<pair<string, string> > rows, string _iden){
    if(rows.size() > 1){
        for(unsigned i=1; i<rows.size(); i++){
            cout << _iden << rows[i].first << " -> " << rows[i].second << "\n";
        }
    }
    else{
        cout << _iden << "Empty!\n";
    }
}

void preprocesing(vector<string> &_inter_node, vector<pair<int, string> > &_inter_sql, int _th){
    _inter_sql.clear();
    vector<string> tmp;

    int i;
    for(i=0; i<_th; i++){
        tmp.push_back("");
    }

    int idx;
    for(i=0; i<(int)_inter_node.size(); i++){
        idx = calcular_hash(_inter_node[i], _th);
        tmp[idx] += ("|" + _inter_node[i]);
    }

    for(i=0; i<(int)tmp.size(); i++){
        if(tmp[i] != ""){
            _inter_sql.push_back({i, tmp[i]});
        }
    }
}

#endif