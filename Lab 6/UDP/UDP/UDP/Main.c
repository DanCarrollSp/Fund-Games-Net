/*
	Simple UDP Server
*/

#include<stdio.h>
#include<winsock2.h>

#pragma comment(lib,"ws2_32.lib")//Winsock Library

#define BUFLEN 512//Max length of buffer for receiving data
#define PORT 8888//The port on which to listen for incoming data

int main()
{
	SOCKET s;//Define socket descriptor
	struct sockaddr_in server, si_other;//structs are for server and clinet addresses
	int slen, recv_len;//Lenght of client address and lenght of received data
	char buf[BUFLEN];//Buffer for storing received data
	WSADATA wsa;//Winsock data structure for initialization

	slen = sizeof(si_other);//sets size of client address struct

	//Initialise winsock
	printf("\nInitialising Winsock...");
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)//Starts winsock
	{
		printf("Failed. Error Code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);//Exit if initialization fails
	}
	printf("Initialised.\n");

	//Create a socket fir UDP
	if ((s = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)//Creates a UDP socket (ipv4)
	{
		printf("Could not create socket : %d", WSAGetLastError());//Error handling for socket creation
	}
	printf("Socket created.\n");

	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;//Accepts incoming data from ip addresses
	server.sin_port = htons(PORT);//Sets the port to listen on

	//Binds the socket to the server address port
	if (bind(s, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)
	{
		printf("Bind failed with error code : %d", WSAGetLastError());//Error handling for binding
		exit(EXIT_FAILURE);//exit if bind fails
	}
	puts("Bind done");//Bind worked

	//keep listening for data
	while (1)
	{
		printf("Waiting for data...");
		fflush(stdout);//Makes sure this is printed before moving on

		//clear the buffer by filling null, it might have previously received data
		memset(buf, '\0', BUFLEN);

		//try to receive some data, this is a blocking call
		if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr*)&si_other, &slen)) == SOCKET_ERROR)
		{
			printf("recvfrom() failed with error code : %d", WSAGetLastError());//Error checking reception
			exit(EXIT_FAILURE);//Exit if data reception fails
		}

		//print details of the client/peer and the data received
		//Changed inet_ntoa to inet_ntop as ntoa is now considered deprecated in windows
		printf("Received packet from %s:%d\n", inet_ntop(si_other.sin_addr), ntohs(si_other.sin_port));
		printf("Data: %s\n", buf);//Prints received data

		//now reply the client with the same data
		if (sendto(s, buf, recv_len, 0, (struct sockaddr*)&si_other, slen) == SOCKET_ERROR)
		{
			printf("sendto() failed with error code : %d", WSAGetLastError());
			exit(EXIT_FAILURE);//Exit if send fails
		}
	}

	closesocket(s);//Close the socket
	WSACleanup();//Cleans up the library

	return 0;
}