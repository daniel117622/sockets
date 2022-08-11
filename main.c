#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <winsock2.h>
 
#include <stdio.h>
#include <stdlib.h>
#include <iphlpapi.h>
#include <ws2tcpip.h>
#include <conio.h>
#include "commands.h"

#define BUFFER_LENGTH 512
#pragma comment(lib,"ws_32.lib")

int main(int argc, char* argv[])
{

    // USER DATA


    int res;
    int sendRes;
    // Initialization block =========
    WSADATA wsaData;
    res = WSAStartup(MAKEWORD(2,2),&wsaData);

    if (res)
    {
        printf("Startup failed.\n");
        return 1;
    }
    
    // ==============================
    // Socket setup
    SOCKET listener;
    listener = socket(AF_INET,SOCK_STREAM, IPPROTO_TCP); // ipv4 ,stream , tcp    
    if (listener == INVALID_SOCKET)
    {
        printf("Error in constructor: Error ID: %d\n",WSAGetLastError());
        WSACleanup();
        return 1;
    }
    // Socket binding
    // struct definition
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr("127.0.0.1");
    address.sin_port = htons(100); // Port definition

    res = bind(listener,(struct sockaddr*)&address,sizeof(address));
    if (res == SOCKET_ERROR)
    {
        printf("Bind failed: %d\n", WSAGetLastError());
        closesocket(listener);
        WSACleanup();
        return 1;
    }

    // listen
    res = listen(listener,SOMAXCONN);
    if (res == SOCKET_ERROR)
    {
        printf("Listen failed: %d\n",WSAGetLastError());
        closesocket(listener);
        WSACleanup();
        return 1;
    }
    printf("Accepting on %s:%d\n","127.0.0.1",100);
    // Handling a client ============

    SOCKET client;
    struct sockaddr_in clientAddr;
    int clientAddrlen;
    client = accept(listener,NULL,NULL); // This function waits for the client
    if (client == INVALID_SOCKET)
    {
        printf("Could not accept clinet: %d\n",WSAGetLastError());
        closesocket(listener);
        WSACleanup();
        return 1;
    }

    // get client information
    getpeername(client,(struct sockaddr*)&clientAddr,&clientAddrlen);
    printf("Client connected at %s:%d\n",inet_ntoa(address.sin_addr),ntohs(address.sin_port)); // Cast it to readable form
    char * welcome = "Welcome to the server :)\n";
    sendRes = send(client,welcome,strlen(welcome),0); // Returns the bytes sent
    if (sendRes != strlen(welcome))
    {
        printf("Error sending: %d\n",WSAGetLastError());
        shutdown(client,SD_BOTH);
        closesocket(client);
    }


    // Main Loop ====================
    const int BUFFLEN = 1024;
    char recBuffer [16];
    
    char instStack[2056];
    int pos = 0;
    do{
        res = recv(client,recBuffer,16,0); // En espera
        
        instStack[pos++] = recBuffer[0];
        instStack[pos] = '\0';
        printf("%d ",instStack[pos-1]);

        // parse_command(instStack);


        char* start_command_ptr = strchr(instStack,'/');
        if (start_command_ptr != NULL) //Look for commands
        {
            if ( !memcmp(start_command_ptr,"/quit\n",5 * sizeof(char)) )
            {
                printf("Closing connection (/quit). \n");
                memset(instStack,0,2056);
                break;
            }
            if ( !memcmp(start_command_ptr,"/help\n",5 * sizeof(char)) )
            {
                printf("\nUser asked for help\n");
                send(client,"\nYou typed /help\n",res,0);

                memset(instStack,0,2056); pos = 0;

            }
            if ( !memcmp(start_command_ptr,"/time\n",5 * sizeof(char)) )
            {
                printf("\nUser requested server time\n");
                send(client,"\nYou requested /time\n",res,0);

                memset(instStack,0,2056); pos = 0;
            }
            if ( !memcmp(start_command_ptr,"/info\n",5 * sizeof(char)) )
            {
                printf("\nShowing server info to user\n");
                send(client,"\nYou typed /info\n",res,0);

                memset(instStack,0,2056); pos = 0;
            }
        }

        /*
        if( !memcmp(recBuffer,"/quit",5 * sizeof(char)) )
        {
            printf("Closing connection (/quit). \n");
            break;
        }
        */
        /* sendRes = send(client,recBuffer,res,0); // Sends back the message */

    } while(1);
    
    printf("========== CONTENTS READ =============\n%s\n",instStack);
    



    res = shutdown(client,SD_BOTH);
    if (res == SOCKET_ERROR)
    {
        printf("Client shutdown failed");
        closesocket(client);
    }
    // Cleanup block ================
    closesocket(listener);
    res = WSACleanup();
    if (res)
    {
        printf("Cleanup failed.\n");
        return 1;
    }
    // ==============================
    system("pause");
    
    return (0);
}