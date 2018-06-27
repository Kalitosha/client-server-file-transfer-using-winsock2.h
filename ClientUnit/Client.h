#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma once //  препроцессорная директива, чтобы файл при компиляции подключался строго один раз
#include "winsock2.h"
#include <vector>
#include <stdint.h>
#pragma comment(lib, "ws2_32.lib") // надо для связывания библиотек

using namespace std;

class Client {
private:
	WSADATA WsaData; // Структура для хранения сведения об инициализации WinSockets
	SOCKET Socket; // сокет
	struct hostent* host;
	SOCKADDR_IN SockAddress;

	uint32_t recSizeInBytes; // Общий размер отправленного файла в байтах
	vector<char> eVec; // тут хранится расширение файла
	uint32_t extensionLength; // длина расширения // кол-во символов

	string newFileName; // локальное имя нового файла

public:
	Client();
	virtual ~Client(); 

	int initWinsock(); // инициализация Winsock
	int initSocket(); // инициализация сокета с которым будет основная работа

	void start();
	void close();

	uint32_t getSizeInBytes(); // возвращает общий размер файла

	vector<char> getExtension(); // Расширение
	/** отправка запроса на сервер */
	bool sendingRequest();
	/** получение данных с сервера */
	void receiveData();
};
