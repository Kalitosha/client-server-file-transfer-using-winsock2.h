#pragma once
#include "WorkWithFile.h"
#include "winsock2.h"
#pragma comment(lib, "ws2_32.lib")

class Server {
private:
	WSADATA WsaData; // ��������� ��� �������� �������� �� ������������� WinSockets
	SOCKET Socket; // �����
	SOCKADDR_IN remoteHost; // ���������, ����������� ����� ���������� �����, � �������� �������������� ����������� ������.
	/* ��������� SOCKADDR_IN ������� ��:
	* sin_family - ��������� ������� (������ ���� AF_INET(internetwork: UDP, TCP, etc.)).
	* sin_port - IP-����.
	* sin_addr - IP-�����.
	* Sin_Zero[8] - ����������, ����� ������� ��������� ������������ � �������� SOCKADDR (������������ ��� �������� IP ������ ����������, ������������ � ����� Windows Sockets).
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