#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>//Windows sockets api - Winsock
#include <ws2tcpip.h>//More advanced IP networking support for windows
#include <stdlib.h>
#include <stdio.h>


// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")//winsock library
#pragma comment (lib, "Mswsock.lib")//sockets extension library
#pragma comment (lib, "AdvApi32.lib")//Advanced windows api (includes security functions)


#define DEFAULT_BUFLEN 512//Data receiving buffer
#define DEFAULT_PORT "27015"//port number for server connection

int __cdecl main(int argc, char** argv)
{
    WSADATA wsaData;//Structure to store winsock data
    SOCKET ConnectSocket = INVALID_SOCKET;//socket descriptor
    struct addrinfo* result = NULL,//stores address info
        * ptr = NULL,//pointer for iterating over address results
        hints;//specifies connection properties
    const char* sendbuf = "this is a test";//data buffer to send
    char recvbuf[DEFAULT_BUFLEN];//buffer to store received data
    int iResult;//stores function results
    int recvbuflen = DEFAULT_BUFLEN;//lenght of received buffer

    // Validate the parameters
    if (argc != 2) {
        printf("usage: %s server-name\n", argv[0]);
        return 1;//exits if incorrect usage
    }

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));//Clear memory for hints
    hints.ai_family = AF_UNSPEC;//Allow IPV4 or IPV6
    hints.ai_socktype = SOCK_STREAM;//use streaming sockets - TCP
    hints.ai_protocol = IPPROTO_TCP;//use TCP protocol

    // Resolve the server address and port
    iResult = getaddrinfo(argv[1], DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();//cleans winsock resources
        return 1;
    }

    // Attempt to connect to an address until one succeeds
    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

        // Create a SOCKET for connecting to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
            ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
            printf("socket failed with error: %ld\n", WSAGetLastError());
            WSACleanup();//cleanup on error
            return 1;
        }

        // Connect to server.
        iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(ConnectSocket);//close on error
            ConnectSocket = INVALID_SOCKET;
            continue;//try next address if available
        }
        break;//Break if the connection succededs
    }

    freeaddrinfo(result);//Free the address info structure

    //Checks if the connection was successful
    if (ConnectSocket == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return 1;
    }

    // Send an initial buffer
    iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
    if (iResult == SOCKET_ERROR) {
        printf("send failed with error: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);//cleanup on error
        WSACleanup();
        return 1;
    }

    printf("Bytes Sent: %ld\n", iResult);

    // shutdown the connection since no more data will be sent
    iResult = shutdown(ConnectSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);//cleanup
        WSACleanup();
        return 1;
    }

    // Receive until the peer closes the connection
    do {

        iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);//Receives data
        if (iResult > 0)
            printf("Bytes received: %d\n", iResult);//Outputs bytes received
        else if (iResult == 0)
            printf("Connection closed\n");//Connection closed
        else
            printf("recv failed with error: %d\n", WSAGetLastError());//error checking

    } while (iResult > 0);//continue until no more data is received

    // cleanup the socket and winsock
    closesocket(ConnectSocket);
    WSACleanup();

    return 0;
}