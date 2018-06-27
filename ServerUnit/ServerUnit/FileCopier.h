#define _CRT_SECURE_NO_WARNINGS
#pragma once

#include <string>
#include <fstream>
#include <stdint.h>
#include <vector>

#define CHUNK_BYTES 1024; // ������ ������

using namespace std;

class FileCopier {
private:
	string iFileName; // ��� �������� �����
	string oFileName; // ��� ��������� �����
	ifstream iFile; // ������� ����
	ofstream oFile; // �������� ����
	uint32_t chunkSize; // ������ ������� (� ������)
	uint32_t fileSize; // ������ ����� � ������
	uint32_t numChunks; // ����� ���������� �������
	bool oneChunk; // true, ���� ��������� ������ ���� �����
	char* currentChunk; // ������ �������� ������ (� ������) //will hold the bytes of the current chunk
public:
	FileCopier();
	virtual ~FileCopier();

	/** �������� ��� �������� ����� */
	void setIFileName();
	/** �������� ��� ��������� ����� (�� ����� ������ ����������) */
	void setOFileName(vector<char> extension);
	/** �������� ��� ��������� ����� (�� ����� ��� � ����������) */
	void setOFileName(char* extension, string fName);
	/** ���� ������ ����� */
	void findFileSize();
	/** ��������� ���-�� ������� */
	void findNumChunks();
	/** ��� ������ ��� ������ ��������, �������� ���� */
	void doWork();
	/** ���������� ������ ����� */
	uint32_t getSize();
	/** ���������� ����� ���-�� ������� */
	uint32_t getNumChunks();
	/** ���������� true, ���� ���� ���� ��������� � ���� ����� */
	bool getOneChunk();
	/** ��������� � ���������� ��� ����� (����������) */
	const char* getExtension();
	/** ���������� ������ ������*/
	uint32_t getChunkSize();
	/** ������ ������ �� ����� */
	void readChunk(char* chunk, uint32_t chunkSize);
	/** ������ ������ � ���� */
	void writeChunk(char* chunk, uint32_t chunkSize);
};