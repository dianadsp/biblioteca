#include <vector>
#include <string>
#include <initializer_list>

using namespace std;

vector<string> tokenizer(string cmd)
{
    vector<string> ans;
    char* tk;
    tk = strtok((char*)cmd.c_str(), " ");
    while(tk != nullptr)
    {
        ans.push_back(tk);
        tk = strtok(nullptr, " ");
    }
    return ans;
}

string join(initializer_list<string> vec)
{
    string ans;
    for(string s: vec)
      ans = ans + s + " ";
    ans.pop_back();
    return ans;
}

string split(vector<string> vec, string sep)
{
    string ans;
    if(!vec.empty())
    {
        ans += vec[0];
        for(int i=1; i<vec.size(); ++i)
            ans += sep + vec[i];
    }
    return ans;
}

string len(string s, int maxlen) // rellena zeros a la izquierda
{
    string zeros(maxlen - to_string(s.length()).length(), '0');
    return zeros + to_string(s.length());
}

string parserGetMsg(string cmd)
{
    string msg;
    int utok = 0;
    for(int i=0; i<cmd.size(); ++i)
    {
        if(i==0 && !isspace(cmd[i]))
          utok += 1;
        else if(isspace(cmd[i-1]) && !isspace(cmd[i]))
          utok += 1;

        if(utok >= 3)
          msg.push_back(cmd[i]);
    }
    return msg;
}


string parserGetField(string pack, int& idx, int bytes)
{
    string sizestr(pack.begin()+idx, 
                   pack.begin()+idx+bytes);
    int size = stoi(sizestr);
    string field(pack.begin()+idx+bytes+1,
                 pack.begin()+idx+bytes+1+size);
    idx += bytes+size+2; // para el prox parseo 
    return field;
}

string errorMsg(string errormsg)
{
    errormsg = "error: " + errormsg;
    string response = join({"4", len(errormsg, 3), errormsg});
    return response;
}

