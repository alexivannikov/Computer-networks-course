#include "pch.h"

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <stdio.h>
#include <winsock2.h>

#pragma comment(lib, "Ws2_32.lib")

#define PORT 8080   // порт сервера

#define ENG 26

char *decrypt(char *str, int encodeKey)
{
	int flag;

	for (int i = 0; i < strlen(str); i++)
	{
		flag = 0;

		if (str[i] >= 'A' && str[i] <= 'Z')
		{
			str[i] = str[i] - (encodeKey % ENG);

			if (str[i] < 'A')
				str[i] = 'Z' - ('A' - str[i]) + 1;

			flag = 1;
		}

		if (str[i] >= 'a' && str[i] <= 'z')
		{
			str[i] = str[i] - (encodeKey % ENG);

			if (str[i] < 'a')
				str[i] = 'z' - ('a' - str[i]) + 1;

			flag = 1;
		}
	}

	return str;
}
int main(int argc, char* argv[])
{
	char buff[1024];

	printf("Server started...\n");

	// Подключение библиотеки 
	if (WSAStartup(0x202, (WSADATA *)&buff[0]))
	{
		printf("WSAStartup error: %d\n",
			WSAGetLastError());
		return -1;
	}

	// Создание сокета
	SOCKET my_sock;
	my_sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (my_sock == INVALID_SOCKET)
	{
		printf("Socket() error: %d\n", WSAGetLastError());
		WSACleanup();
		return -1;
	}

	// Связывание сокета с локальным адресом 
	sockaddr_in local_addr;
	local_addr.sin_family = AF_INET;
	local_addr.sin_addr.s_addr = INADDR_ANY;
	local_addr.sin_port = htons(PORT);

	if (bind(my_sock, (sockaddr *)&local_addr,
		sizeof(local_addr)))
	{
		printf("bind error: %d\n", WSAGetLastError());
		closesocket(my_sock);
		WSACleanup();
		return -1;
	}

	// Обработка пакетов, присланных клиентами
	while (1)
	{
		sockaddr_in client_addr;
		int client_addr_size = sizeof(client_addr);
		int bsize = recvfrom(my_sock, &buff[0],
			sizeof(buff) - 1, 0,
			(sockaddr *)&client_addr, &client_addr_size);
		if (bsize == SOCKET_ERROR)
			printf("recvfrom() error: %d\n",
				WSAGetLastError());

		// Определение IP-адреса клиента и прочие атрибуты
		HOSTENT *hst;
		hst = gethostbyaddr((char *)
			&client_addr.sin_addr, 4, AF_INET);
		printf("+%s [%s:%d] new DATAGRAM!\n",
			(hst) ? hst->h_name : "Unknown host",
			inet_ntoa(client_addr.sin_addr),
			ntohs(client_addr.sin_port));

		// Добавление завершающего нуля
		buff[bsize] = 0;

		// Вывод на экран 
 		printf("Client => Server:%s\n", &buff[0]);
		
		//Расшифровка сообщения
		decrypt(buff, 2);

		// Посылка датаграммы клиенту
		sendto(my_sock, &buff[0], bsize, 0,
			(sockaddr *)&client_addr, sizeof(client_addr));	
	}
	
	return 0;
}