#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

#define SOCKET_ERROR        -1
#define BUFFER_SIZE         100
#define HOST_NAME_SIZE      255
#define PORT				5000

int  main (int argc, char* argv[])
{
    int hSocket;
    struct hostent * pHostInfo;
    struct sockaddr_in Address;
    long nHostAddress;
    char pBuffer[BUFFER_SIZE];
	char url[BUFFER_SIZE];
    unsigned nReadAmount;
    char strHostName[HOST_NAME_SIZE];
    int nHostPort;
	int clientID;
	int invalidURL = 1;
	int i;

    if(argc < 2)	{
		printf("\nUsage: client <server IP/Host Name>\n");
		return 0;
	}
    else	{
		strcpy (strHostName,argv[1]);
		nHostPort = PORT;
	}

    hSocket = socket (AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if (hSocket == SOCKET_ERROR)	{
        printf("\nCould not make a socket\n");
        return 1;
    }
	
    pHostInfo = gethostbyname (strHostName);
    memcpy (&nHostAddress, pHostInfo->h_addr, pHostInfo->h_length);

    Address.sin_addr.s_addr = nHostAddress;
    Address.sin_port = htons (nHostPort);
    Address.sin_family = AF_INET;

    if (connect (hSocket, (struct sockaddr *) &Address, sizeof (Address)) == SOCKET_ERROR)	{
        printf("\nCould not connect to host\n");
        return 1;
    }
	
	read (hSocket, pBuffer, BUFFER_SIZE);
	sscanf (pBuffer, "%d %s", &clientID, pBuffer);
	
	printf ("\nReceived Client ID from server as: Client %d\n", clientID);
    printf ("\nReceived from Server:\n%s\n", pBuffer);
    
	for (;;) {
		printf ("\nClient %d -- Enter the URL or type \"exit\" to quit >> ", clientID);
		memset (pBuffer, 0, BUFFER_SIZE);
		scanf ("%s", pBuffer);
		strcpy (url, pBuffer);
		invalidURL = 1;
		
		if (strcmp (pBuffer, "exit") == 0) {
			memset (pBuffer, 0, BUFFER_SIZE);
			write (hSocket, pBuffer, BUFFER_SIZE);
			break;
		}
		
		write (hSocket, pBuffer, BUFFER_SIZE);
		printf ("\nWriting \"%s\" to server", pBuffer);

		memset (pBuffer, 0, BUFFER_SIZE);
		nReadAmount = read (hSocket, pBuffer, BUFFER_SIZE);
		printf ("\n%s\n", pBuffer);
		
		while (read (hSocket, pBuffer, BUFFER_SIZE) > 0) {
			pBuffer[BUFFER_SIZE] = '\0';
			
			if (strcmp (pBuffer, "EOF") == 0) {
				break;
			}
			
			for (i = 0; i < BUFFER_SIZE; i++)
				printf ("%c", pBuffer[i]);
			
			fflush (stdout);
			memset (pBuffer, 0, BUFFER_SIZE);
			invalidURL = 0;
		}
		if (invalidURL)
			printf ("Server was unable to resolve the URL '%s'\n", url);
		else
			printf ("\nResponse completed from Server for URL: %s\n", url);
	}
	
	if (close (hSocket) == SOCKET_ERROR)	{
		printf("\nCould not close socket\n");
		return 0;
	}
}