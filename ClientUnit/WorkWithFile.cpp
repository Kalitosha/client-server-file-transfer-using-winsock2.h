#include "stdafx.h"
#include "WorkWithFile.h"
#include <iostream>
using namespace std;
/*
	1. �������� ��� ����� fileNames
	2. ���������� ������ ������
	3. ���� ������ �����
	4. ��������� ����������� ���������� �������
	5. ��������� �����, ���������� ��� � ����, ��������� 5 ����� ���� �� ��������� ��� ������
	*/

WorkWithFile::WorkWithFile() {
	//������������� ������� ������
	chunkSize = CHUNK_BYTES;
	oneChunk = true;
}

WorkWithFile::~WorkWithFile() {}

/** �������� ��� �������� ����� */
void WorkWithFile::setIFileName() {
	iFileName = "test.txt";
	cout << "File name (with extension): "<< iFileName << "\n";	
	//cin >> iFileName;
}

/** ���������� ��� �����*/
const char* WorkWithFile::getIFileName() {
	const char* c = iFileName.c_str(); // ������ �� ������ ��-� �������
	char* ret = new char[strlen(c)]; // ������ ������-��������� ������������ ��������� �� ������ ����������
	strcpy(ret, c); // strcpy(char* result, const char* source); �������� �� source � result 
	return ret;
}

/** �������� ��� ��������� ����� (�� ����� ��� � ����������) */
void WorkWithFile::setOFileName(/*char* extension, string fName*/ string fName) {
	oFileName = fName;
	//oFileName = fName + extension;
	oFile = ofstream(oFileName, ios::out | ios::binary); // �������������� ����, ��������� ��� ������ � �������� ������
}

/** ���� ������ ����� */
void WorkWithFile::findFileSize() {
	iFile = ifstream(iFileName, ios::in | ios::binary | ios::ate); // ��������� �� ������ � �������� ������ � ������������� ��������� � ����� �����
	if (iFile.is_open()) { // ���������, ������ �� �����
		fileSize = iFile.tellg(); // �.�. ��������� ��������� � ����� �����, ��� ������� ��� ������ // tellg() ���������� ������� ������� ������
		iFile.seekg(0, ios::beg); // ����� ��������� �� ������ ����� 
		// seekg(offset, rpos)	������������� ������������� ������� ������ 
		// beg �������� �������� ������������ ������ �����
	}
	else {
		cout << "File couldn't be found!\n";		
		setIFileName();
		findFileSize();
	}
}

/** ��������� ���-�� ������� */
void WorkWithFile::findNumChunks() {
	if (fileSize > chunkSize) {
		oneChunk = false;
		numChunks = fileSize / chunkSize;
	}
	else {
		numChunks = 1;
	}
}

/** ��� ������ ��� ������ ��������, �������� ���� */
void WorkWithFile::doWork() {
	oFile = ofstream(oFileName, ios::out | ios::binary); // ��������� ��� ������ � �������� ������
	if (oneChunk) { // ���� ���� ���������� � ���� �����
		char* currentChunk = new char[fileSize]; // ������ ��� ������
		iFile.read(currentChunk, fileSize); // ��������� �����
		oFile.write(currentChunk, fileSize); // ���������� �����
		delete[] currentChunk; // ������� �����
	}
	else { // ���� ��������� ��������� ������� (�������������, ��� ��������� ���� ����� ���������� ������)
		uint32_t chunksWritten = 0; // ���������� ������
		while (chunksWritten <= numChunks) { // ���� �� �������� ��� ������
			if (chunksWritten != numChunks) {
				currentChunk = new char[chunkSize]; // ������ ��� ������
				iFile.read(currentChunk, chunkSize); // ��������� �����
				oFile.write(currentChunk, chunkSize); // ���������� �����
				delete[] currentChunk; // ������� �����
			}
			else { //���� �� ��������� �������� (�� ��������� ������)
				uint32_t lastChunkSize = fileSize - (chunkSize * numChunks); // ���������� ������ ���������� ������
				currentChunk = new char[lastChunkSize]; 
				iFile.read(currentChunk, lastChunkSize);
				oFile.write(currentChunk, lastChunkSize);
				delete[] currentChunk;
			}
			cout << "chunks: " << chunksWritten << "/" << numChunks << "\n";
			chunksWritten++;
		}
	}
}

/** ���������� ������ ����� */
uint32_t WorkWithFile::getSize() {
	return fileSize;
}

/** ��������� � ���������� ��� ����� (����������) */
const char* WorkWithFile::getExtension() {
	int indexOf = iFileName.find_last_of("."); // find_last_of() ���������� ������ ���������� ������� ������ ������ ������, ������� ��������� � �������� �����
	string ext; // ����������
	for (int i = indexOf; i < strlen(iFileName.c_str()); i++) { // ������������ ����������, ������� � �����
		// strlen(string) ���� ����� ������ 
		// c_str() ���������� ��������� �� ���������� ������ � �-�����
		ext.push_back(iFileName[i]); // �������� ����������
	}
	const char* c = ext.c_str(); // ������ �� ������ ��-� �������
	char* ret = new char[strlen(c)]; // ������ ������-��������� ������������ ��������� �� ������ ����������
	strcpy(ret, c); // strcpy(char* result, const char* source); �������� �� source � result 
	return ret;
}

/** ���������� ����� ���-�� ������� */
uint32_t WorkWithFile::getNumChunks() {
	return numChunks;
}

/** ���������� true, ���� ���� ���� ��������� � ���� ����� */
bool WorkWithFile::getOneChunk() {
	return oneChunk;
}

/** ���������� ������ ������*/
uint32_t WorkWithFile::getChunkSize() {
	uint32_t chunkSize = CHUNK_BYTES;
	return chunkSize;
}

/** ������ ������ �� ����� */
void WorkWithFile::readChunk(char* chunk, uint32_t chunkSize) {
	iFile.read(chunk, chunkSize);
}

/** ������ ������ � ���� */
void WorkWithFile::writeChunk(char* chunk, uint32_t chunkSize) {
	oFile.write(chunk, chunkSize);
}

void WorkWithFile::closeFile() {
	iFile.close();
	oFile.close();
}