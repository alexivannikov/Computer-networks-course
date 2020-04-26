#include "pch.h"

#pragma comment(lib,"ws2_32.lib")
#pragma warning(disable: 4996)

#include <iostream>
#include <WinSock2.h>
#include <string>
#include <fstream>

using namespace std;

SOCKET connection;

int main() {
	WSAData wsaData;
	WORD Dllversion MAKEWORD(2, 1);

	if (WSAStartup(Dllversion, &wsaData) != 0) 
	{
		cout << "Error!" << endl;
		exit(1);
	}

	SOCKADDR_IN addr;
	int sizeofaddr = sizeof(addr);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(8080);
	addr.sin_family = AF_INET;

	connection = socket(AF_INET, SOCK_STREAM, NULL);

	cout << "Connecting to server 127.0.0.1 : 8080..."<< endl;

	if (connect(connection, (SOCKADDR*)&addr, sizeof(addr)) != 0)
	{
		cout << "Error: connect to server failed!" << endl;
		return 1;
	}

	cout << "Connected\n" << endl;
	
	char msg[256];
	
	recv(connection, msg, sizeof(msg), NULL);
	
	cout << msg << endl;

	string filename;

	while (true)
	{
		cout << "Input name of file which you want to transmit to server" << endl;
		getline(cin, filename);

		ifstream fin;
		fin.open(filename);
		string file_name = filename;

		long file_size;
		fin.seekg(0, ios::end);
		file_size = fin.tellg();

		int st = file_name.find('.');
		int last_b = sizeof(file_name) - st;
		
		string ext = file_name.substr(st, last_b);

		cout << file_size << endl;

		send(connection, file_name.c_str(), 256, NULL);
		Sleep(5);
		send(connection, ext.c_str(), 10, NULL);
		Sleep(5);
		send(connection, (char*)&file_size, sizeof(long), NULL);
		Sleep(5);

		char str[1024];

		fin.seekg(0, ios::beg);
		
		int line = 0;
		
		while (!fin.eof())
		{
			fin.getline(str, 1024);
			line++;
		}

		char* buff;

		fin.seekg(0, ios::beg);
		
		int i = 0;
		int size = 0;

		while (!fin.eof())
		{
			if ((i + 1) * 1024 > file_size)
			{
				size = file_size - i * 1024 - line;
			}
			else
			{
				size = 1024;
			}
			
			buff = new char[size + 1];
			fin.read(buff, 1024);
			buff[size] = '\0';
			cout << buff << endl;
			send(connection, buff, 1024, NULL);
			Sleep(3);
			i++;
		}

		fin.close();
	}
	system("pause");
	closesocket(connection);
	WSACleanup();
	return 0;
}