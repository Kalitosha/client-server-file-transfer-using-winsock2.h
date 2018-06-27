#include "stdafx.h"
#include "Server.h"
#include <stdint.h>
#include <iostream>
#include <sstream>

using namespace std;

Server::Server() {
	initWinsock();
	initSocket();
	start();
}
Server::~Server() {}

int Server::initWinsock() {
	if (WSAStartup(MAKEWORD(2, 2), &WsaData) != 0) { //����� ������������� ���������� �������(Ws2_32.dll) ���������
		cout << "Winsock could not initialize!\n";
		WSACleanup(); // �������� ���������� Ws2_32.dll
		return 1;
	}
	return 0;
}

int Server::initSocket() {
	Socket = socket(AF_INET, // 2 // internetwork: UDP, TCP, etc. // ��������� ���������� ������������ ������
					SOCK_STREAM, // 1 // stream socket (� ���������� �����������) // ��� ������ 
					IPPROTO_TCP); // ��������. ����� ����: TCP, UDP, ��� Windows ���������� ����� ���� 0(������ ����� �������������� �������� �� ���������) � �.�.
	if (Socket == INVALID_SOCKET) {
		cout << "Socket creation failed!";
		WSACleanup();
		return 1;
	}

	remoteHost.sin_family = AF_INET; // ��������� ������� internetwork: UDP, TCP, etc. (����������)
	remoteHost.sin_addr.s_addr = INADDR_ANY; // ip ����� ���������� �����
	remoteHost.sin_port = htons(DEFAULT_PORT); // ���� 

	// bind() ������������ �������� ��� ������������ ������ �����.
	if (bind(Socket, // ����� - ����������, ������� ������ ������� �������
			(SOCKADDR*)(&remoteHost), // ��������� �� ��������� ����� ������
			sizeof(remoteHost)) == SOCKET_ERROR) { // ������ ������ (� ������) ��������� ������
		cout << "Was unable to bind socket!\n";
		WSACleanup();
		return 2;
	}
	return 0;
}

void Server::start() {
	listen(Socket, 1); // 1 - ���-�� �������� �����, ������� ����� ���� ������� �� ����� ������������. ��� ����� �� ����� ��������� � ����� ����������
	SOCKET tempSocket = SOCKET_ERROR; // ��������� �����
	while (tempSocket == SOCKET_ERROR) {
		cout << "Waiting for incoming connections...\n";
		tempSocket = accept(Socket, NULL, NULL); // accept() ������������ �������� ��� �������� ����� �� �����
	}
	Socket = tempSocket;
	cout << "\n\nClient has connected!\n\n";

	WorkWithFile f;
	//TODO ��� ���� ��������� � ������������ ������ �������
	string str = receiveRequest();
	
	f.setIFileName(str); // �������� ��� �������� �����
	if (f.findFileSize() == false) // ��������� ������ �����
		sendError();
	else {
		f.findNumChunks(); // ��������� ���������� �������
		sendData(f); // �������� ������
	}
	f.closeFile();
	close();
}

void Server::close() {
	shutdown(Socket, SD_SEND); // shutdown() ������������ ��� ������������ �������� ���� ��� ����� ������ � �������
	closesocket(Socket); // �������� ����������
	WSACleanup();
}

void Server::sendError() {
	//TODO 
}

char* Server::receiveRequest() {

	char* fName = new char[MAX_PATH]; // ��� �����, ����������� ��������� ����� 260 �������� � �����������
	uint32_t bytesReceived = 0; // ���������� �����
	cout << "Receiving request from client...";
	while (bytesReceived != strlen(fName) + 1) {
		bytesReceived = recv(Socket, fName, strlen(fName) + 1, 0);
		if (bytesReceived == strlen(fName) + 1) {
			cout << "Request successfully received!\n";
			fName[bytesReceived] = '\0'; // ������� ����� ������, ���� ���������� ���� ������� ��������
			return fName;
		}
	}
}

void Server::sendData(WorkWithFile& f) {
	/*
	0. �������� ����� �����
	1. �������� ������� ����� (� ������)
	//2. �������� ����� ���������� (� ������)
	//3. �������� ����������
	4. ��������� ������ �� ������, ���������� �� �� ������
	*/

	//0. �������� ����� �����
	uint32_t bytesSent = 0; // ���������� ������������ ��-��
	const char* fName = f.getIFileName(); // �������� ���
	cout << "Sending FileName...";
	while (bytesSent != strlen(fName) + 1) {
		bytesSent = send(Socket, // ������ ���������� socket'�, ����� ������� ���������� ������
			fName, // ��������� �� ������� ������, ���������� ������������ ������
			strlen(fName) + 1,  // ������ ����� (� ������) ������������ ������
			0); // ������������ ���������� ���������� ������. ��� ������� �������� ����� ��������� ����� send ��������� ���������� ���������� ������ write.
		if (bytesSent == strlen(fName) + 1) {
			cout << "FileName successfully sent!\n";
		}
	}

	//1. �������� ������� ����� (� ������)

	uint32_t fileSize = f.getSize(); // ���������� ������ �����
	bytesSent = 0; // ���������� ������������ ��-��

	cout << "Sending filesize...";
	while (bytesSent != sizeof(fileSize)) {
		bytesSent = send(Socket, // ������ ���������� socket'�, ����� ������� ���������� ������
					(char*)(&fileSize),  // ��������� �� ������� ������, ���������� ������������ ������
					sizeof(fileSize), // ������ ����� (� ������) ������������ ������
					0); // ������������ ���������� ���������� ������. ��� ������� �������� ����� ��������� ����� send ��������� ���������� ���������� ������ write.
		if (bytesSent == sizeof(fileSize)) {
			cout << "Filesize successfully sent!\n";
		}
	}
	/*
	//2. �������� ����� ���������� (� ������)

	uint32_t extensionLength = strlen(f.getExtension()); // ����� ���������� // strlen ���������� ����� ������
	bytesSent = 0; // ���������� ��������

	cout << "Sending file extension length...";
	while (bytesSent != sizeof(extensionLength)) {
		bytesSent = send(Socket, (char*)(&extensionLength), sizeof(extensionLength), 0);
		if (bytesSent == sizeof(extensionLength)) {
			cout << "File extension length successfuly sent!\n";
		}
	}
	
	//3. �������� ���������� � ������ c-�����

	const char* extension = f.getExtension(); // �������� ����������
	bytesSent = 0; // ���������� ��������

	cout << "Sending file extension...";
	while (bytesSent != sizeof(extension)) {
		bytesSent = send(Socket, extension, sizeof(extension), 0);
		if (bytesSent == sizeof(extension)) {
			cout << "File extension successfully sent!\n";
		}
	}
	*/

	//4. �������� ������� � �������
	// ��� ���������� ������ ����� ������������ �������������� ������ � ������������� �� �� ������� ������� (��� �������� ���)

	uint32_t chunksSent = 0; // ���-�� ������������ �������
	char* currentChunk = new char[f.getChunkSize()]; // ������ ��� ����������� ������
	uint32_t totalBytesSent = 0; // ��������� ���-�� ������������ ��-��
	cout << "\nSending file...";
	while (chunksSent <= f.getNumChunks()) { // ���� ���-�� ������������ ������� <= ������ ����� �������
		f.readChunk(currentChunk, f.getChunkSize()); // ������ ����� �� �����
		uint32_t numBytesSent = 0; // ���-�� ������������ ����
		uint32_t numBytesLeft = f.getChunkSize(); // ���������� ���-�� ����
		uint32_t curSendBytes; // �������� ���-�� �������. ����

		while (numBytesSent < numBytesLeft) {
			curSendBytes = send(Socket, currentChunk + numBytesSent, numBytesLeft, 0);
			if (curSendBytes > 0 && curSendBytes <= f.getChunkSize()) {
				numBytesSent += curSendBytes;
				numBytesLeft -= curSendBytes;
				totalBytesSent += curSendBytes;
			}
		}
		chunksSent++;
		//cout << "\rSent: " << totalBytesSent  << "/" << fileSize  << " MB";
	}
	cout << "File successfully sent!\n";
	delete[] currentChunk;
}