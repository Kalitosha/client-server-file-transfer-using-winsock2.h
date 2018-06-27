#pragma once
#include "WorkWithFile.h"
#include "winsock2.h"
#pragma comment(lib, "ws2_32.lib")

class Server {
private:
	WSADATA WsaData; // Структура для хранения сведения об инициализации WinSockets
	SOCKET Socket; // сокет
	SOCKADDR_IN remoteHost; // структура, указывающая адрес удаленного хоста, к которому осуществляется подключение сокета.
	/* структура SOCKADDR_IN состоит из:
	* sin_family - Семейство адресов (должно быть AF_INET(internetwork: UDP, TCP, etc.)).
	* sin_port - IP-порт.
	* sin_addr - IP-адрес.
	* Sin_Zero[8] - дополнения, чтобы сделать структуру совпадаеющей с размером SOCKADDR (используется для хранения IP адреса компьютера, участвующего в связи Windows Sockets).
	*/
public:
	const u_short DEFAULT_PORT = 4444;
	Server();
	virtual ~Server();
	int initWinsock();
	int initSocket();
	void start();
	void close();
	void sendData(WorkWithFile& f);
	void sendError();
	char* receiveRequest();
};