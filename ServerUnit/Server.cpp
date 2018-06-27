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
	if (WSAStartup(MAKEWORD(2, 2), &WsaData) != 0) { //старт использования библиотеки сокетов(Ws2_32.dll) процессом
		cout << "Winsock could not initialize!\n";
		WSACleanup(); // выгрузка библиотеки Ws2_32.dll
		return 1;
	}
	return 0;
}

int Server::initSocket() {
	Socket = socket(AF_INET, // 2 // internetwork: UDP, TCP, etc. // семейство протоколов создаваемого сокета
					SOCK_STREAM, // 1 // stream socket (с постоянным соединением) // тип сокета 
					IPPROTO_TCP); // Протокол. может быть: TCP, UDP, для Windows приложений может быть 0(значит будет использоваться заданный по умолчанию) и т.д.
	if (Socket == INVALID_SOCKET) {
		cout << "Socket creation failed!";
		WSACleanup();
		return 1;
	}

	remoteHost.sin_family = AF_INET; // Семейство адресов internetwork: UDP, TCP, etc. (межсетевой)
	remoteHost.sin_addr.s_addr = INADDR_ANY; // ip адрес удаленного хоста
	remoteHost.sin_port = htons(DEFAULT_PORT); // порт 

	// bind() используется сервером для присваивания сокету имени.
	if (bind(Socket, // сокет - дескриптор, который данная функция именует
			(SOCKADDR*)(&remoteHost), // указатель на структуру имени сокета
			sizeof(remoteHost)) == SOCKET_ERROR) { // задает размер (в байтах) структуры данных
		cout << "Was unable to bind socket!\n";
		WSACleanup();
		return 2;
	}
	return 0;
}

void Server::start() {
	listen(Socket, 1); // 1 - кол-во запросов связи, которое может быть принято на сокет одновременно. Это число не имеет отношения к числу соединений
	SOCKET tempSocket = SOCKET_ERROR; // временный сокет
	while (tempSocket == SOCKET_ERROR) {
		cout << "Waiting for incoming connections...\n";
		tempSocket = accept(Socket, NULL, NULL); // accept() используется сервером для принятия связи на сокет
	}
	Socket = tempSocket;
	cout << "\n\nClient has connected!\n\n";

	WorkWithFile f;
	//TODO тут надо принимать и обрабатывать запрос клиента
	string str = receiveRequest();
	
	f.setIFileName(str); // получаем имя входного файла
	if (f.findFileSize() == false) // вычисляем размер файла
		sendError();
	else {
		f.findNumChunks(); // вычисляем количество пакетов
		sendData(f); // передача данных
	}
	f.closeFile();
	close();
}

void Server::close() {
	shutdown(Socket, SD_SEND); // shutdown() используется для немедленного закрытия всех или части связей с сокетом
	closesocket(Socket); // закрытие соединения
	WSACleanup();
}

void Server::sendError() {
	//TODO 
}

char* Server::receiveRequest() {

	char* fName = new char[MAX_PATH]; // имя файла, максимально возможная длина 260 символов с шасширением
	uint32_t bytesReceived = 0; // почученные байты
	cout << "Receiving request from client...";
	while (bytesReceived != strlen(fName) + 1) {
		bytesReceived = recv(Socket, fName, strlen(fName) + 1, 0);
		if (bytesReceived == strlen(fName) + 1) {
			cout << "Request successfully received!\n";
			fName[bytesReceived] = '\0'; // признак конца строки, если расширение было успешно получено
			return fName;
		}
	}
}

void Server::sendData(WorkWithFile& f) {
	/*
	0. отправка имени файла
	1. отправка размера файла (в байтах)
	//2. отправка длины расширения (в байтах)
	//3. отправка расширения
	4. разбиваем данные на пакеты, отправляем их по одному
	*/

	//0. отправка имени файла
	uint32_t bytesSent = 0; // количество отправленной ин-фы
	const char* fName = f.getIFileName(); // получаем имя
	cout << "Sending FileName...";
	while (bytesSent != strlen(fName) + 1) {
		bytesSent = send(Socket, // задает дескриптор socket'а, через который посылаются данные
			fName, // указывает на область памяти, содержащую передаваемые данные
			strlen(fName) + 1,  // задает длину (в байтах) передаваемых данных
			0); // модифицирует исполнение системного вызова. При нулевом значении этого аргумента вызов send полностью аналогичен системному вызову write.
		if (bytesSent == strlen(fName) + 1) {
			cout << "FileName successfully sent!\n";
		}
	}

	//1. отправка размера файла (в байтах)

	uint32_t fileSize = f.getSize(); // определяем размер файла
	bytesSent = 0; // количество отправленной ин-фы

	cout << "Sending filesize...";
	while (bytesSent != sizeof(fileSize)) {
		bytesSent = send(Socket, // задает дескриптор socket'а, через который посылаются данные
					(char*)(&fileSize),  // указывает на область памяти, содержащую передаваемые данные
					sizeof(fileSize), // задает длину (в байтах) передаваемых данных
					0); // модифицирует исполнение системного вызова. При нулевом значении этого аргумента вызов send полностью аналогичен системному вызову write.
		if (bytesSent == sizeof(fileSize)) {
			cout << "Filesize successfully sent!\n";
		}
	}
	/*
	//2. отправка длины расширения (в байтах)

	uint32_t extensionLength = strlen(f.getExtension()); // длина расширения // strlen возвращает длину строки
	bytesSent = 0; // сбрасываем значение

	cout << "Sending file extension length...";
	while (bytesSent != sizeof(extensionLength)) {
		bytesSent = send(Socket, (char*)(&extensionLength), sizeof(extensionLength), 0);
		if (bytesSent == sizeof(extensionLength)) {
			cout << "File extension length successfuly sent!\n";
		}
	}
	
	//3. отправка расширения в строке c-стиля

	const char* extension = f.getExtension(); // получаем расширение
	bytesSent = 0; // сбрасываем значение

	cout << "Sending file extension...";
	while (bytesSent != sizeof(extension)) {
		bytesSent = send(Socket, extension, sizeof(extension), 0);
		if (bytesSent == sizeof(extension)) {
			cout << "File extension successfully sent!\n";
		}
	}
	*/

	//4. отправка пакетов с данными
	// для последнего пакета можно игнорировать дополнительные данные и анализировать их на стороне клиента (это упрощает код)

	uint32_t chunksSent = 0; // кол-во отправленных пакетов
	char* currentChunk = new char[f.getChunkSize()]; // массив для содержимого пакета
	uint32_t totalBytesSent = 0; // суммарное кол-во отправленной ин-фы
	cout << "\nSending file...";
	while (chunksSent <= f.getNumChunks()) { // пока кол-во отправленных пакетов <= общему числу пакетов
		f.readChunk(currentChunk, f.getChunkSize()); // читаем пакет из файла
		uint32_t numBytesSent = 0; // кол-во отправленных байт
		uint32_t numBytesLeft = f.getChunkSize(); // оставшееся кол-во байт
		uint32_t curSendBytes; // буферное кол-во отправл. байт

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