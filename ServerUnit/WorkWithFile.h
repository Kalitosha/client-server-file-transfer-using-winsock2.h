#define _CRT_SECURE_NO_WARNINGS
#pragma once
#include <string>
#include <fstream>
#include <stdint.h>
#include <vector>
#define CHUNK_BYTES 1024; // ������ ������
using namespace std;

class WorkWithFile {
private:
	string iFileName; // ��� �������� �����
	string oFileName; // ��� ��������� �����
	ifstream iFile; // ������� ����
	ofstream oFile; // �������� ����
	uint32_t chunkSize; // ������ ������� (� ������)
	uint32_t fileSize; // ������ ����� � ������
	uint32_t numChunks; // ����� ���������� �������
	bool oneChunk; // true, ���� ��������� ������ ���� �����
	char* currentChunk; // ������ �������� ������ (� ������)
public:
	WorkWithFile();
	virtual ~WorkWithFile();

	/** �������� ��� �������� ����� */
	void setIFileName();
	void setIFileName(string fName);

	/** �������� ��� ��������� ����� (�� ����� ��� � ����������) */
	void setOFileName(char* extension, string fName);

	/** ���������� ��� �����*/
	const char* WorkWithFile::getIFileName();

	/** ���� ������ ����� */
	bool findFileSize();

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

	void closeFile();
};