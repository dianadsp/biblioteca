all: client.exe server.exe
	@echo Terminado
	
client.exe: client.cpp
	g++ client.cpp -o client.exe -std=c++11 -pthread

server.exe: server.cpp
	g++ server.cpp -o server.exe -std=c++11 -pthread


clear: 
	rm ./client.exe ./server.exe
