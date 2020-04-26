#include "pch.h"

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <stdio.h>
#include <winsock2.h>

#pragma comment(lib, "Ws2_32.lib")

#define PORT 8080
#define SERVERADDR "127.0.0.1"

#define ENG 26

void encrypt(char *str, int encodeKey)
{
	int flag;

	for (int i = 0; i < strlen(str); i++)
	{
		flag = 0; 

		if (str[i] >= 'A' && str[i] <= 'Z')
		{
			str[i] = str[i] + (encodeKey % ENG);
			
			if (str[i] > 'Z') 
				str[i] = 'A' + (str[i] - 'Z') - 1;
			
			flag = 1;
		}

		if (str[i] >= 'a' && str[i] <= 'z')
		{
			str[i] = str[i] + (encodeKey % ENG);
			
			if (str[i] > 'z') 
				str[i] = 'a' + (str[i] - 'z') - 1;

			flag = 1;
		}
	}
}

int main()
{
	char buff[10 * 1014];
	
	printf("Input your message\n");

	// Иницилизация библиотеки Winsocks
	if (WSAStartup(0x202, (WSADATA *)&buff[0]))
	{
		printf("WSAStartup error: %d\n",
			WSAGetLastError());
		return -1;
	}

	// Открытие сокета
	SOCKET my_sock = socket(AF_INET, SOCK_DGRAM, 0);
	
	if (my_sock == INVALID_SOCKET)
	{
		printf("socket() error: %d\n", WSAGetLastError());
		WSACleanup();
		return -1;
	}

	// Обмен сообщений с сервером
	HOSTENT *hst;
	sockaddr_in dest_addr;

	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons(PORT);

	// Определение IP-адреса узла
	if (inet_addr(SERVERADDR))
		dest_addr.sin_addr.s_addr = inet_addr(SERVERADDR);
	else
		if (hst = gethostbyname(SERVERADDR))
			dest_addr.sin_addr.s_addr = ((unsigned long **)
				hst->h_addr_list)[0][0];
		else
		{
			printf("Unknown host: %d\n", WSAGetLastError());
			closesocket(my_sock);
			WSACleanup();
			return -1;
		}

	while (1)
	{
		// Чтение сообщения с клавиатуры
		printf("\nServer <= Client:");
		
		fgets(&buff[0], sizeof(buff) - 1, stdin);
		
		if (!strcmp(&buff[0], "quit\n")) 
			break;

		//Шифрование сообщения
		encrypt(buff, 2);

		// Передача сообщений на сервер
		sendto(my_sock, buff, strlen(buff), 0, (sockaddr *)&dest_addr, sizeof(dest_addr));

		// Прием сообщения с сервера
		sockaddr_in server_addr;
		int server_addr_size = sizeof(server_addr);

		int n = recvfrom(my_sock, buff, sizeof(buff) - 1, 0, (sockaddr *)&server_addr, &server_addr_size);

		if (n == SOCKET_ERROR)
		{
			printf("recvfrom() error:"\
				"%d\n", WSAGetLastError());
			closesocket(my_sock);
			WSACleanup();
			return -1;
		}

		buff[n] = 0;

		// Вывод принятого с сервера сообщения на экран
		printf("Server => Client:%s", &buff[0]);
	}

	// Выход
	closesocket(my_sock);
	WSACleanup();

	return 0;
}