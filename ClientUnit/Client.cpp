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
		Функция WSAStartup должна быть первой функцией Windows Sockets, вызываемой приложением или DLL. 
		Он позволяет приложению или DLL определять требуемую версию Windows Sockets и получать сведения 
		о конкретной реализации Windows Sockets. Приложение или DLL могут выполнять только дополнительные 
		функции Windows Sockets после успешного вызова WSAStartup.
	*/
	if (WSAStartup(MAKEWORD(2, 2), &WsaData) != 0) { //старт использования библиотеки сокетов(Ws2_32.dll) процессом
		// MAKEWORD задает версию и под.версию интерфейса Winsock. Возможные версии - 1.0, 1.1, 2.0, 2.2...
		// структура WsaData при возврате из функции содержит информацию о проинициализированной версии WinsockAPI
		cout << "Winsock initialization failed!\n";
		WSACleanup(); // выгрузка библиотеки Ws2_32.dll
		return 1;
	}
	return 0;
}
int Client::initSocket() {
	Socket = socket(AF_INET, // internetwork: UDP, TCP, etc. // семейство протоколов создаваемого сокета
		SOCK_STREAM, // stream socket (с постоянным соединением) // тип сокета 
		IPPROTO_TCP); // Протокол. может быть: TCP, UDP, для Windows может быть 0(значит будет использоваться заданный по умолчанию)
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
		hostName = s.c_str(); // c_str() возвращает указатель на символьный массив, который содержит строку объекта стринг
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
	//connect устанавливает соединение с указанным сокетом.
	if (connect(Socket, //Дескриптор, обозначающий несоединенный сокет.
		(SOCKADDR*)(&SockAddress),  // Указатель на структуру sockaddr, к которой должно быть установлено подключение 
		sizeof(SockAddress)) != 0) { // Длина (в байтах) структуры sockaddr, на которую указывает адрес сокета.
		cout << "Failed to connect with server!\n";
		WSACleanup();
		return 3;
	}
	cout << "\n\nSuccessfuly connected to server at: " << hostName << " : " << portNum << "\n\n";
	return 0;
}

void Client::start() {
	if (sendingRequest() == true) // отправляем запрос на сервер	
		 receiveData(); // получаем данные от сервера
	close();
}

void Client::close() {
	shutdown(Socket, SD_SEND); // shutdown() используется для немедленного закрытия всех или части связей с сокетом
	closesocket(Socket); // закрытие соединения
	WSACleanup();
}
//TODO sending request
/** отправка запроса на сервер */
bool Client::sendingRequest() {

	uint32_t bytesSent = 0; // количество отправленной ин-фы
	WorkWithFile f;
	f.setIFileName(); // задаем имя файла
	const char* fName = f.getIFileName(); // получаем имя
	cout << "Sending request...";
	while (bytesSent != strlen(fName) + 1) {
		bytesSent = send(Socket, // задает дескриптор socket'а, через который посылаются данные
			fName, // указывает на область памяти, содержащую передаваемые данные
			strlen(fName) + 1,  // задает длину (в байтах) передаваемых данных
			0); // модифицирует исполнение системного вызова. При нулевом значении этого аргумента вызов send полностью аналогичен системному вызову write.
		if (bytesSent == strlen(fName)+1) {
			cout << "Request successfully sent!\n";
			return true;
		}
	}
}

/** возвращает общий размер файла (в байтах) */
uint32_t Client::getSizeInBytes() {
	return recSizeInBytes;
}

/** возвращает расширение файла */
vector<char> Client::getExtension() {
	return eVec;
}

/** получаем данные от сервера */
void Client::receiveData() {  
	/*
	* 0. получаем имя файла
	* 1. получаем размер файла в байтах
	* 2. получеам длину расширения файла
	* 3.получаем расширение в виде строки string
	* 4. получаем даннные пакетами в двоичном формате
	*/
	
	//0. получаем имя файла
	WorkWithFile f;
	char* fName = new char[MAX_PATH]; // имя файла, максимально возможная длина 260 символов с шасширением
	uint32_t bytesReceived = 0; // почученные байты
	cout << "Receiving fileName from client...";
	while (bytesReceived != strlen(fName) + 1) {
		bytesReceived = recv(Socket, fName, strlen(fName) + 1, 0);
		if (bytesReceived == strlen(fName) + 1) {
			cout << "FileName successfully received!\n";
			fName[bytesReceived] = '\0'; // признак конца строки, если расширение было успешно получено
			f.setOFileName(/*extension, newFileName*/fName);
		}
	}
	
	//1. получаем размер файла в байтах

	uint32_t fileSize = 0; // буфер для записи размера файла
	bytesReceived = 0; // почученные байты
	cout << "Receiving filesize...";
	while (bytesReceived != sizeof(fileSize)) { // пока весь файл не получен
		// recv(int socket, char* buf, int len, int flags); считывает данные из сокета. Функция возвращает число считанных байтов, -1 в случае ошибки, 0 при закрытом соединении
		// Оно сигнализирует об отсутствии записанных в сокет процессом-поставщиком данных.
		bytesReceived = recv(Socket, // сокет из которого читаются данные
							(char*)(&fileSize), // адрес буфера для записи читаемых данных
							sizeof(fileSize), // длина этого буфера
							0); // комбинация битовых флагов, управляющих режимами чтения // при нуле считанные данные удаляются из сокета.
		if (bytesReceived == sizeof(fileSize)) { // если размер данных в буфере совпадает с количеством принятых данных, передача закончилась
			cout << "Filesize successfully received!\n";
		}
	}
	/*
	//2. получеам длину расширения файла

	uint32_t extensionLength = 0; // длина расширения
	bytesReceived = 0; // сброс переменной для следующего считывания recv()

	cout << "Receiving file extension length...";
	while (bytesReceived != sizeof(fileSize)) {
		bytesReceived = recv(Socket, (char*)(&extensionLength), sizeof(extensionLength), 0);
		if (bytesReceived == sizeof(extensionLength)) {
			cout << "File extension length successfully received!\n";
		}
	}
	
	//3.получаем расширение в виде строки с-стиля

	char* extension = new char[extensionLength]; // расширение
	bytesReceived = 0; 
	cout << "Receiving file extension...";
	while (bytesReceived != sizeof(extension)) {
		bytesReceived = recv(Socket, extension, sizeof(extension), 0);
		if (bytesReceived == sizeof(extension)) {
			cout << "File extension successfully received!\n";
			extension[extensionLength] = '\0'; // признак конца строки, если расширение было успешно получено
		}
	}
	*/

	//4. получаем даннные пакетами в двоичном формате

	// получение одного пакета и запись в файл
	uint32_t chunksReceived = 0; // принятые пакеты
	char* currentChunk = new char[f.getChunkSize()]; //массив символов находящихся в пакете
	uint32_t lastChunkSize = fileSize - (f.getChunkSize() * (fileSize / f.getChunkSize())); // вычисляем размер последнего пакета (скорее всего полупустого)
	uint32_t allBytesRec = 0; // все полученные байты
	cout << "\nReceiving file...";
	while (chunksReceived <= (fileSize / f.getChunkSize())) {
		uint32_t numBytesReceived = 0; // количество принятых байт // буфер
		uint32_t n; // количество принятых данных при последнем соединении

		// собираем данные в пакет
		while (numBytesReceived < f.getChunkSize()) { // считываем данные пока пакет не заполнится
			n = recv(Socket, currentChunk + numBytesReceived, f.getChunkSize(), 0); // считываем данные с сервера
			if (n > 0 && n <= f.getChunkSize()) { // если все прошло успешно // если полученный размер меньше или равен размеру пакета
				numBytesReceived += n; // обновляем кол-во принятых байт
				allBytesRec += n;
			}
		}
		if (chunksReceived != (fileSize / f.getChunkSize())) { // запись пакета в файл
			f.writeChunk(currentChunk, numBytesReceived); // записываем пакет размерности последнего приема
		}
		else {
			f.writeChunk(currentChunk, lastChunkSize); // сюда на последней итерации // это будет недопакет (полупустой)
		}		
		chunksReceived++; // пакет был успешно принят и записан, можно приступать к следующему
	}
	delete[] currentChunk; // очищаем память
	cout << "File successfully received!\n";
}