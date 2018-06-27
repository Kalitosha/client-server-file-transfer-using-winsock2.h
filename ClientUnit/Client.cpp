#include "stdafx.h"
#include "Client.h"
#include <iostream>
#include <string>
#include "WorkWithFile.h"
#include <iomanip>

Client::Client() {
	recSizeInBytes = 0; 
	if (initWinsock() != 0 ) close();
	else {
		if (initSocket() != 0) close();
		else start();
	}
}
Client::~Client() {}

int Client::initWinsock() {
	/**
		������� WSAStartup ������ ���� ������ �������� Windows Sockets, ���������� ����������� ��� DLL. 
		�� ��������� ���������� ��� DLL ���������� ��������� ������ Windows Sockets � �������� �������� 
		� ���������� ���������� Windows Sockets. ���������� ��� DLL ����� ��������� ������ �������������� 
		������� Windows Sockets ����� ��������� ������ WSAStartup.
	*/
	if (WSAStartup(MAKEWORD(2, 2), &WsaData) != 0) { //����� ������������� ���������� �������(Ws2_32.dll) ���������
		// MAKEWORD ������ ������ � ���.������ ���������� Winsock. ��������� ������ - 1.0, 1.1, 2.0, 2.2...
		// ��������� WsaData ��� �������� �� ������� �������� ���������� � ��������������������� ������ WinsockAPI
		cout << "Winsock initialization failed!\n";
		WSACleanup(); // �������� ���������� Ws2_32.dll
		return 1;
	}
	return 0;
}
int Client::initSocket() {
	Socket = socket(AF_INET, // internetwork: UDP, TCP, etc. // ��������� ���������� ������������ ������
		SOCK_STREAM, // stream socket (� ���������� �����������) // ��� ������ 
		IPPROTO_TCP); // ��������. ����� ����: TCP, UDP, ��� Windows ����� ���� 0(������ ����� �������������� �������� �� ���������)
	if (Socket == INVALID_SOCKET) {
		cout << "Socket creation failed!\n";
		WSACleanup();
		return 1;
	}
	//TODO newFileName
	//cout << "Please enter new filename: ";
	//cin >> newFileName;
	///////////////////////////////////////////////////////////
	bool noAgain = true;
	string s = "127.0.0.1";
	int portNum = 4444;
	char const* hostName = s.c_str();
	host = gethostbyname(hostName);
	/*while (noAgain) {
		noAgain = false;
		cout << "Please enter server's hostname or IP address: ";
		cin >> s;
		hostName = s.c_str(); // c_str() ���������� ��������� �� ���������� ������, ������� �������� ������ ������� ������
		if ((host = gethostbyname(hostName)) == NULL) {
			cout << "Failed to resolve hostname!\n";
			noAgain = true;
			continue;
		}
		cout << "Please enter server's port number: ";
		cin >> portNum;
		if (portNum > 65535) {
			cout << "Failed to resolve port!\n";
			noAgain = true;
			continue;
		}
	}*/
	///////////////////////////////////////////////////////////
	SockAddress.sin_port = htons(portNum);
	SockAddress.sin_family = AF_INET;
	SockAddress.sin_addr.s_addr = *((unsigned long*)host->h_addr);
	//connect ������������� ���������� � ��������� �������.
	if (connect(Socket, //����������, ������������ ������������� �����.
		(SOCKADDR*)(&SockAddress),  // ��������� �� ��������� sockaddr, � ������� ������ ���� ����������� ����������� 
		sizeof(SockAddress)) != 0) { // ����� (� ������) ��������� sockaddr, �� ������� ��������� ����� ������.
		cout << "Failed to connect with server!\n";
		WSACleanup();
		return 3;
	}
	cout << "\n\nSuccessfuly connected to server at: " << hostName << " : " << portNum << "\n\n";
	return 0;
}

void Client::start() {
	if (sendingRequest() == true) // ���������� ������ �� ������	
		 receiveData(); // �������� ������ �� �������
	close();
}

void Client::close() {
	shutdown(Socket, SD_SEND); // shutdown() ������������ ��� ������������ �������� ���� ��� ����� ������ � �������
	closesocket(Socket); // �������� ����������
	WSACleanup();
}
//TODO sending request
/** �������� ������� �� ������ */
bool Client::sendingRequest() {

	uint32_t bytesSent = 0; // ���������� ������������ ��-��
	WorkWithFile f;
	f.setIFileName(); // ������ ��� �����
	const char* fName = f.getIFileName(); // �������� ���
	cout << "Sending request...";
	while (bytesSent != strlen(fName) + 1) {
		bytesSent = send(Socket, // ������ ���������� socket'�, ����� ������� ���������� ������
			fName, // ��������� �� ������� ������, ���������� ������������ ������
			strlen(fName) + 1,  // ������ ����� (� ������) ������������ ������
			0); // ������������ ���������� ���������� ������. ��� ������� �������� ����� ��������� ����� send ��������� ���������� ���������� ������ write.
		if (bytesSent == strlen(fName)+1) {
			cout << "Request successfully sent!\n";
			return true;
		}
	}
}

/** ���������� ����� ������ ����� (� ������) */
uint32_t Client::getSizeInBytes() {
	return recSizeInBytes;
}

/** ���������� ���������� ����� */
vector<char> Client::getExtension() {
	return eVec;
}

/** �������� ������ �� ������� */
void Client::receiveData() {  
	/*
	* 0. �������� ��� �����
	* 1. �������� ������ ����� � ������
	* 2. �������� ����� ���������� �����
	* 3.�������� ���������� � ���� ������ string
	* 4. �������� ������� �������� � �������� �������
	*/
	
	//0. �������� ��� �����
	WorkWithFile f;
	char* fName = new char[MAX_PATH]; // ��� �����, ����������� ��������� ����� 260 �������� � �����������
	uint32_t bytesReceived = 0; // ���������� �����
	cout << "Receiving fileName from client...";
	while (bytesReceived != strlen(fName) + 1) {
		bytesReceived = recv(Socket, fName, strlen(fName) + 1, 0);
		if (bytesReceived == strlen(fName) + 1) {
			cout << "FileName successfully received!\n";
			fName[bytesReceived] = '\0'; // ������� ����� ������, ���� ���������� ���� ������� ��������
			f.setOFileName(/*extension, newFileName*/fName);
		}
	}
	
	//1. �������� ������ ����� � ������

	uint32_t fileSize = 0; // ����� ��� ������ ������� �����
	bytesReceived = 0; // ���������� �����
	cout << "Receiving filesize...";
	while (bytesReceived != sizeof(fileSize)) { // ���� ���� ���� �� �������
		// recv(int socket, char* buf, int len, int flags); ��������� ������ �� ������. ������� ���������� ����� ��������� ������, -1 � ������ ������, 0 ��� �������� ����������
		// ��� ������������� �� ���������� ���������� � ����� ���������-����������� ������.
		bytesReceived = recv(Socket, // ����� �� �������� �������� ������
							(char*)(&fileSize), // ����� ������ ��� ������ �������� ������
							sizeof(fileSize), // ����� ����� ������
							0); // ���������� ������� ������, ����������� �������� ������ // ��� ���� ��������� ������ ��������� �� ������.
		if (bytesReceived == sizeof(fileSize)) { // ���� ������ ������ � ������ ��������� � ����������� �������� ������, �������� �����������
			cout << "Filesize successfully received!\n";
		}
	}
	/*
	//2. �������� ����� ���������� �����

	uint32_t extensionLength = 0; // ����� ����������
	bytesReceived = 0; // ����� ���������� ��� ���������� ���������� recv()

	cout << "Receiving file extension length...";
	while (bytesReceived != sizeof(fileSize)) {
		bytesReceived = recv(Socket, (char*)(&extensionLength), sizeof(extensionLength), 0);
		if (bytesReceived == sizeof(extensionLength)) {
			cout << "File extension length successfully received!\n";
		}
	}
	
	//3.�������� ���������� � ���� ������ �-�����

	char* extension = new char[extensionLength]; // ����������
	bytesReceived = 0; 
	cout << "Receiving file extension...";
	while (bytesReceived != sizeof(extension)) {
		bytesReceived = recv(Socket, extension, sizeof(extension), 0);
		if (bytesReceived == sizeof(extension)) {
			cout << "File extension successfully received!\n";
			extension[extensionLength] = '\0'; // ������� ����� ������, ���� ���������� ���� ������� ��������
		}
	}
	*/

	//4. �������� ������� �������� � �������� �������

	// ��������� ������ ������ � ������ � ����
	uint32_t chunksReceived = 0; // �������� ������
	char* currentChunk = new char[f.getChunkSize()]; //������ �������� ����������� � ������
	uint32_t lastChunkSize = fileSize - (f.getChunkSize() * (fileSize / f.getChunkSize())); // ��������� ������ ���������� ������ (������ ����� �����������)
	uint32_t allBytesRec = 0; // ��� ���������� �����
	cout << "\nReceiving file...";
	while (chunksReceived <= (fileSize / f.getChunkSize())) {
		uint32_t numBytesReceived = 0; // ���������� �������� ���� // �����
		uint32_t n; // ���������� �������� ������ ��� ��������� ����������

		// �������� ������ � �����
		while (numBytesReceived < f.getChunkSize()) { // ��������� ������ ���� ����� �� ����������
			n = recv(Socket, currentChunk + numBytesReceived, f.getChunkSize(), 0); // ��������� ������ � �������
			if (n > 0 && n <= f.getChunkSize()) { // ���� ��� ������ ������� // ���� ���������� ������ ������ ��� ����� ������� ������
				numBytesReceived += n; // ��������� ���-�� �������� ����
				allBytesRec += n;
			}
		}
		if (chunksReceived != (fileSize / f.getChunkSize())) { // ������ ������ � ����
			f.writeChunk(currentChunk, numBytesReceived); // ���������� ����� ����������� ���������� ������
		}
		else {
			f.writeChunk(currentChunk, lastChunkSize); // ���� �� ��������� �������� // ��� ����� ��������� (����������)
		}		
		chunksReceived++; // ����� ��� ������� ������ � �������, ����� ���������� � ����������
	}
	delete[] currentChunk; // ������� ������
	cout << "File successfully received!\n";
}