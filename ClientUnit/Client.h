#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma once //  ��������������� ���������, ����� ���� ��� ���������� ����������� ������ ���� ���
#include "winsock2.h"
#include <vector>
#include <stdint.h>
#pragma comment(lib, "ws2_32.lib") // ���� ��� ���������� ���������

using namespace std;

class Client {
private:
	WSADATA WsaData; // ��������� ��� �������� �������� �� ������������� WinSockets
	SOCKET Socket; // �����
	struct hostent* host;
	SOCKADDR_IN SockAddress;

	uint32_t recSizeInBytes; // ����� ������ ������������� ����� � ������
	vector<char> eVec; // ��� �������� ���������� �����
	uint32_t extensionLength; // ����� ���������� // ���-�� ��������

	string newFileName; // ��������� ��� ������ �����

public:
	Client();
	virtual ~Client(); 

	int initWinsock(); // ������������� Winsock
	int initSocket(); // ������������� ������ � ������� ����� �������� ������

	void start();
	void close();

	uint32_t getSizeInBytes(); // ���������� ����� ������ �����

	vector<char> getExtension(); // ����������
	/** �������� ������� �� ������ */
	bool sendingRequest();
	/** ��������� ������ � ������� */
	void receiveData();
};
