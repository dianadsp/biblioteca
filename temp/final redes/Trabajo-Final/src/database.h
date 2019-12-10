#ifndef _DATABASE_H_
#define _DATABASE_H_

#include <iostream>
#include <sys/stat.h>
#include <string>
#include <vector>
#include <sqlite3.h>

using namespace std;

vector<vector<string> > result_select;
bool header;
vector<string> tmp;

static int callback(void* data, int argc, char** argv, char** azColName){//me retorna mi matriz con resultados
	int i;
	if(header){//pongo cabecera(id nodo value)
		tmp.clear();
		tmp.resize(argc);

		for(i=0; i<argc; i++)
			tmp[i] = azColName[i];

		result_select.push_back(tmp);
		header = false;
	}

	for(i=0; i<argc; i++){//pongo los valores
		if(argv[i]){
			tmp[i] = argv[i];
		}
		else{
			tmp[i] = "NULL";
		}
	}	
	result_select.push_back(tmp);
	return 0;
}

class CDatabase{
	public:
		sqlite3* m_sqlite;
		string m_nombre;

		CDatabase(string);
		~CDatabase();
		bool execute(string); 
		bool execute(string, vector<vector<string> > &);
};

inline bool existeBD(const string& name){
	struct stat buffer;
	return (stat (name.c_str(), &buffer) == 0);
}

CDatabase::CDatabase(string nombre){
	this->m_nombre = nombre;
	bool existe = existeBD(m_nombre);

    int bd=sqlite3_open(m_nombre.c_str(), &m_sqlite); 
    if(bd){
        cerr << "Error al crear BD" << sqlite3_errmsg(m_sqlite) << "\n";
    }
    else{
    	if(existe){
    		cout << "BD ya existe"<<endl;
    	}
    	else{
    		cout << "Se creó base de datos"<<endl;
			string sql;
			sql="CREATE TABLE NODE("
				"ID 		INTEGER PRIMARY KEY AUTOINCREMENT, "
				"NAME 		TEXT 	NOT NULL UNIQUE, "
				"ATTRIBUTE 	TEXT"
				");";
				
			execute(sql);
			sql="CREATE TABLE RELATION("
					"ID 		INTEGER PRIMARY KEY AUTOINCREMENT, "
					"NAME1 		TEXT                NOT NULL, "
					"NAME2 		TEXT                NOT NULL"
					");";
			execute(sql);
    	}
    }
}


bool CDatabase::execute(string query){
	char* mensaje_error; 
	int bd = sqlite3_exec(m_sqlite, query.c_str(), NULL, 0, &mensaje_error); 

    if (bd!=SQLITE_OK) { 
        cout<<mensaje_error<< "\n";
        sqlite3_free(mensaje_error); 
        return false;
    }
    return true;
}

bool CDatabase::execute(string _sql, vector<vector<string> > &_res){//solo para funcion select
	header = true;//almacena los nombres de las columnas solo una vez
	result_select.clear();
	_res.clear();

	string data("callback function");
	int rq = sqlite3_exec(m_sqlite, _sql.c_str(), callback, (void*)data.c_str(), NULL);

	if(rq != SQLITE_OK){
		cerr << "Error select\n";
		return false;
	}
	_res = result_select;

	return true;
}

CDatabase::~CDatabase(){
	sqlite3_close(m_sqlite);
}

#endif