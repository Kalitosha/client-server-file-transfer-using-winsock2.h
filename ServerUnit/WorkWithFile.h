#define _CRT_SECURE_NO_WARNINGS
#pragma once
#include <string>
#include <fstream>
#include <stdint.h>
#include <vector>
#define CHUNK_BYTES 1024; // размер пакета
using namespace std;

class WorkWithFile {
private:
	string iFileName; // имя входного файла
	string oFileName; // имя выходного файла
	ifstream iFile; // входной файл
	ofstream oFile; // выходной файл
	uint32_t chunkSize; // размер пакетов (в байтах)
	uint32_t fileSize; // Размер файла в байтах
	uint32_t numChunks; // общее количество пакетов
	bool oneChunk; // true, если требуется только один пакет
	char* currentChunk; // размер текущего пакета (в байтах)
public:
	WorkWithFile();
	virtual ~WorkWithFile();

	/** получаем имя входного файла */
	void setIFileName();
	void setIFileName(string fName);

	/** получаем имя выходного файла (на входе имя и расширение) */
	void setOFileName(char* extension, string fName);

	/** возвращает имя файла*/
	const char* WorkWithFile::getIFileName();

	/** ищем размер файла */
	bool findFileSize();

	/** вычисляем кол-во пакетов */
	void findNumChunks();

	/** как только все данные доступны, копируем файл */
	void doWork();

	/** возвращает размер файла */
	uint32_t getSize();

	/** возвращает общее кол-во пакетов */
	uint32_t getNumChunks();

	/** возвращает true, если весь файл умещается в один пакет */
	bool getOneChunk();

	/** извлекает и возвращает тип файла (расширение) */
	const char* getExtension();

	/** возвращает размер пакета*/
	uint32_t getChunkSize();

	/** чтение пакета из файла */
	void readChunk(char* chunk, uint32_t chunkSize);

	/** запись пакета в файл */
	void writeChunk(char* chunk, uint32_t chunkSize);

	void closeFile();
};