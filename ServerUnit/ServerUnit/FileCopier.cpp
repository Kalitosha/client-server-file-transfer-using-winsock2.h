#include "stdafx.h"
#include "FileCopier.h"
#include <iostream>
using namespace std;
/*
	1. получаем имя файла fileNames
	2. определяем размер пакета
	3. ищем размер файла
	4. вычисляем необходимое количество пакетов
	5. сохраняем пакет, записываем его в файл, повторяем 5 пункт пока не запишутся все данные
	*/

FileCopier::FileCopier() {
	//инициализация размера пакета
	chunkSize = CHUNK_BYTES;
	oneChunk = true;
}

FileCopier::~FileCopier() {}

/** получаем имя входного файла */
void FileCopier::setIFileName() {
	cout << "File name (with extension): ";
	cin >> iFileName;
}
/** получаем имя выходного файла (на входе только расширение) */
void FileCopier::setOFileName(vector<char> extension) { 
	cout << "File name: ";
	cin >> oFileName;
	for (int i = 0; i < extension.size(); i++) { // добавляем расширение
		oFileName.push_back(extension[i]);
	}
	oFile = ofstream(oFileName, ios::out | ios::binary); // инициализируем файл, открываем для записи в двоичном режиме
}

/** получаем имя выходного файла (на входе имя и расширение) */
void FileCopier::setOFileName(char* extension, string fName) {
	oFileName = fName + extension;
	oFile = ofstream(oFileName, ios::out | ios::binary); // инициализируем файл, открываем для записи в двоичном режиме
}

/** ищем размер файла */
void FileCopier::findFileSize() {
	iFile = ifstream(iFileName, ios::in | ios::binary | ios::ate); // открываем на чтение в двоичном режиме и устанавлмваем указатель в конец файла
	if (iFile.is_open()) { // проверяем, открыт ли поток
		fileSize = iFile.tellg(); // т.к. узазатель находится в конце файла, это покажет его размер // tellg() возвращает текущую позицию чтения
		iFile.seekg(0, ios::beg); // сброс указателя на начало файла 
		// seekg(offset, rpos)	устанавливает относительную позицию чтения 
		// beg смещение задается относительно начала файла
	}
	else {
		cout << "File could not be found!\n";
	}
}

/** вычисляем кол-во пакетов */
void FileCopier::findNumChunks() {
	if (fileSize > chunkSize) {
		oneChunk = false;
		numChunks = fileSize / chunkSize;
	}
	else {
		numChunks = 1;
	}
}

/** как только все данные доступны, копируем файл */
void FileCopier::doWork() {
	oFile = ofstream(oFileName, ios::out | ios::binary); // открываем для записи в двоичном режиме

	if (oneChunk) { // если файл помещается в один пакет
		char* currentChunk = new char[fileSize]; // массив для пакета
		iFile.read(currentChunk, fileSize); // считываем пакет
		oFile.write(currentChunk, fileSize); // записываем пакет
		delete[] currentChunk; // удаляем пакет
	}
	else { // если требуется несколько пакетов (гарантируется, что последний блок имеет правильный размер)
		uint32_t chunksWritten = 0; // записанные пакеты
		while (chunksWritten <= numChunks) { // пока не записаны все пакеты

			if (chunksWritten != numChunks) {
				currentChunk = new char[chunkSize]; // массив для пакета
				iFile.read(currentChunk, chunkSize); // считываем пакет
				oFile.write(currentChunk, chunkSize); // записываем пакет
				delete[] currentChunk; // удаляем пакет
			}
			else { //сюда на последней итерации (на последнем пакете)
				uint32_t lastChunkSize = fileSize - (chunkSize * numChunks); // определяем размер последнего пакета
				currentChunk = new char[lastChunkSize]; 
				iFile.read(currentChunk, lastChunkSize);
				oFile.write(currentChunk, lastChunkSize);
				delete[] currentChunk;
			}
			cout << "chunks: " << chunksWritten << "/" << numChunks << "\n"; //TODO можно выкинуть
			chunksWritten++;
		}
	}
}

/** возвращает размер файла */
uint32_t FileCopier::getSize() {
	return fileSize;
}

/** извлекает и возвращает тип файла (расширение) */
const char* FileCopier::getExtension() {
	int indexOf = iFileName.find_last_of("."); // find_last_of() возвращает индекс последнего символа внутри данной строки, который совпадает с символом точки
	string ext; // расширение

	for (int i = indexOf; i < strlen(iFileName.c_str()); i++) { // обрабатываем расширение, начиная с точки
		// strlen(string) ищет длину строки 
		// c_str() возвращает указатель на символьный массив в с-стиле
		ext.push_back(iFileName[i]); // собираем расширение
	}

	const char* c = ext.c_str(); // ссылка на первый эл-т массива?

	char* ret = new char[strlen(c)]; // делаем массив-указатель длины указателя на начало расширения
	strcpy(ret, c); //TODO НАХРЕНА // strcpy(char* result, const char* source); копирует из source в result 
	return ret;
}

/** возвращает общее кол-во пакетов */
uint32_t FileCopier::getNumChunks() {
	return numChunks;
}

/** возвращает true, если весь файл умещается в один пакет */
bool FileCopier::getOneChunk() {
	return oneChunk;
}

/** возвращает размер пакета*/
uint32_t FileCopier::getChunkSize() {
	uint32_t chunkSize = CHUNK_BYTES;
	return chunkSize;
}

/** чтение пакета из файла */
void FileCopier::readChunk(char* chunk, uint32_t chunkSize) {
	iFile.read(chunk, chunkSize);
}
/** запись пакета в файл */
void FileCopier::writeChunk(char* chunk, uint32_t chunkSize) { 
	oFile.write(chunk, chunkSize);
}