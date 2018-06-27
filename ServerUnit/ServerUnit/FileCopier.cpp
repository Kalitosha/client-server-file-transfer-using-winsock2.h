#include "stdafx.h"
#include "FileCopier.h"
#include <iostream>
using namespace std;
/*
	1. �������� ��� ����� fileNames
	2. ���������� ������ ������
	3. ���� ������ �����
	4. ��������� ����������� ���������� �������
	5. ��������� �����, ���������� ��� � ����, ��������� 5 ����� ���� �� ��������� ��� ������
	*/

FileCopier::FileCopier() {
	//������������� ������� ������
	chunkSize = CHUNK_BYTES;
	oneChunk = true;
}

FileCopier::~FileCopier() {}

/** �������� ��� �������� ����� */
void FileCopier::setIFileName() {
	cout << "File name (with extension): ";
	cin >> iFileName;
}
/** �������� ��� ��������� ����� (�� ����� ������ ����������) */
void FileCopier::setOFileName(vector<char> extension) { 
	cout << "File name: ";
	cin >> oFileName;
	for (int i = 0; i < extension.size(); i++) { // ��������� ����������
		oFileName.push_back(extension[i]);
	}
	oFile = ofstream(oFileName, ios::out | ios::binary); // �������������� ����, ��������� ��� ������ � �������� ������
}

/** �������� ��� ��������� ����� (�� ����� ��� � ����������) */
void FileCopier::setOFileName(char* extension, string fName) {
	oFileName = fName + extension;
	oFile = ofstream(oFileName, ios::out | ios::binary); // �������������� ����, ��������� ��� ������ � �������� ������
}

/** ���� ������ ����� */
void FileCopier::findFileSize() {
	iFile = ifstream(iFileName, ios::in | ios::binary | ios::ate); // ��������� �� ������ � �������� ������ � ������������� ��������� � ����� �����
	if (iFile.is_open()) { // ���������, ������ �� �����
		fileSize = iFile.tellg(); // �.�. ��������� ��������� � ����� �����, ��� ������� ��� ������ // tellg() ���������� ������� ������� ������
		iFile.seekg(0, ios::beg); // ����� ��������� �� ������ ����� 
		// seekg(offset, rpos)	������������� ������������� ������� ������ 
		// beg �������� �������� ������������ ������ �����
	}
	else {
		cout << "File could not be found!\n";
	}
}

/** ��������� ���-�� ������� */
void FileCopier::findNumChunks() {
	if (fileSize > chunkSize) {
		oneChunk = false;
		numChunks = fileSize / chunkSize;
	}
	else {
		numChunks = 1;
	}
}

/** ��� ������ ��� ������ ��������, �������� ���� */
void FileCopier::doWork() {
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
			cout << "chunks: " << chunksWritten << "/" << numChunks << "\n"; //TODO ����� ��������
			chunksWritten++;
		}
	}
}

/** ���������� ������ ����� */
uint32_t FileCopier::getSize() {
	return fileSize;
}

/** ��������� � ���������� ��� ����� (����������) */
const char* FileCopier::getExtension() {
	int indexOf = iFileName.find_last_of("."); // find_last_of() ���������� ������ ���������� ������� ������ ������ ������, ������� ��������� � �������� �����
	string ext; // ����������

	for (int i = indexOf; i < strlen(iFileName.c_str()); i++) { // ������������ ����������, ������� � �����
		// strlen(string) ���� ����� ������ 
		// c_str() ���������� ��������� �� ���������� ������ � �-�����
		ext.push_back(iFileName[i]); // �������� ����������
	}

	const char* c = ext.c_str(); // ������ �� ������ ��-� �������?

	char* ret = new char[strlen(c)]; // ������ ������-��������� ����� ��������� �� ������ ����������
	strcpy(ret, c); //TODO ������� // strcpy(char* result, const char* source); �������� �� source � result 
	return ret;
}

/** ���������� ����� ���-�� ������� */
uint32_t FileCopier::getNumChunks() {
	return numChunks;
}

/** ���������� true, ���� ���� ���� ��������� � ���� ����� */
bool FileCopier::getOneChunk() {
	return oneChunk;
}

/** ���������� ������ ������*/
uint32_t FileCopier::getChunkSize() {
	uint32_t chunkSize = CHUNK_BYTES;
	return chunkSize;
}

/** ������ ������ �� ����� */
void FileCopier::readChunk(char* chunk, uint32_t chunkSize) {
	iFile.read(chunk, chunkSize);
}
/** ������ ������ � ���� */
void FileCopier::writeChunk(char* chunk, uint32_t chunkSize) { 
	oFile.write(chunk, chunkSize);
}