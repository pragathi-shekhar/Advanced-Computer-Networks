#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <fcntl.h>

#define SOCKET_ERROR        -1
#define BUFFER_SIZE         100
#define QUEUE_SIZE          500
#define PORT 			    5000

struct threadData {
	int socket;
	int threadID;
};
struct threadData tData[BUFFER_SIZE];

void padding (char * str) {
	while (strlen (str) < BUFFER_SIZE)
		strcat (str, " ");
}

void * tProxyJob (void * threadArg)	{
	struct threadData * this;
	int hSocket, nReadAmount, exitStatus;
	char pBuffer[BUFFER_SIZE];
	char fName[BUFFER_SIZE];
	char request[BUFFER_SIZE * 3];
	char line[BUFFER_SIZE + 1];
	char url[BUFFER_SIZE];
	FILE * fp = NULL;
	
	sleep (1);
	
	this = (struct threadData *) threadArg;
	hSocket = this->socket;

	memset (pBuffer, 0, BUFFER_SIZE);
	sprintf (pBuffer, "%d %s", this->threadID, "Connection_Established");
	write (hSocket, pBuffer, BUFFER_SIZE);
	
	printf ("\nConnection Established with Client %d\n", this->threadID);

	for (;;) {
		read (hSocket, pBuffer, BUFFER_SIZE);
		printf ("\nRequest from Client %d: %s\n", this->threadID, pBuffer);
		strcpy (url, pBuffer);
		
		if (strcmp (pBuffer, "exit") == 0) {
			pthread_exit(NULL);
		
			if (close (hSocket) == SOCKET_ERROR)	{
				printf ("\nCould not close socket\n");
				return;
			}
			return;
		}
		
		sprintf (request, "curl --request GET %s", pBuffer, this->threadID);
		printf ("Request Command: '%s'\nExecuting the request.......\n", request);
		fp = popen (request, "r");
		if (fp == NULL) {
			printf ("Server abnormality..\nTerminating client %d connection...\n", this->threadID);
			pthread_exit (NULL);
			if (close (hSocket) == SOCKET_ERROR)	
				printf ("\nCould not close socket\n");
			return;
		}
		
		memset (pBuffer, 0, BUFFER_SIZE);
		sprintf (pBuffer, "%s", "Content of the URL's response being sent:");
		write (hSocket, pBuffer, BUFFER_SIZE);
		
		printf ("\nContent received and sending to client %d for %s from the web servers:\n\n", this->threadID, url);
		memset (line, 0, BUFFER_SIZE);
		while (fgets (line, BUFFER_SIZE - 1, fp) != NULL) {
			line[BUFFER_SIZE] = '\0';
			padding (line);
			printf("%s", line);
			write (hSocket, line, BUFFER_SIZE);
			memset (line, 0, BUFFER_SIZE);
		}
		
		memset (line, 0, BUFFER_SIZE);
		sprintf (line, "%s", "EOF");
		write (hSocket, line, BUFFER_SIZE);
		
		pclose (fp);
		printf ("\n\n\nServed Client %d with its recent URL request: %s\nWaiting for next request.....\n\n", this->threadID, url);
		
		// sleep (5);
	}
}

int main (int argc, char* argv[])
{
	int hSocket, hServerSocket;
	int thID = 0, i = 0;
	struct hostent * pHostInfo;
	struct sockaddr_in Address;
	int nAddressSize = sizeof (struct sockaddr_in);
	char pBuffer[BUFFER_SIZE];
	pthread_t threads[BUFFER_SIZE];

	printf ("Connecting via port: %d\n", PORT);
	printf ("Designed to handle %d clients at one instance of time..\n", BUFFER_SIZE);
	printf ("Starting server......\n");
	hServerSocket = socket (AF_INET, SOCK_STREAM, 0);

	if (hServerSocket == SOCKET_ERROR)	{
		printf ("\nCould not make a socket\n");
		return 0;
	}

	Address.sin_addr.s_addr = INADDR_ANY;
	Address.sin_port = htons (PORT);
	Address.sin_family = AF_INET;

	if (bind (hServerSocket, (struct sockaddr *) &Address, sizeof (Address)) == SOCKET_ERROR)	{
		printf ("\nCould not connect to host\n");
		return 0;
	}

	getsockname (hServerSocket, (struct sockaddr *) &Address, (socklen_t *) &nAddressSize);
	if (listen (hServerSocket, QUEUE_SIZE) == SOCKET_ERROR)    {
		printf ("\nCould not listen\n");
		return 0;
	}

	for(;;)    {
		i = (i + 1) % BUFFER_SIZE;
		
		hSocket = accept (hServerSocket, (struct sockaddr*) &Address, (socklen_t *) &nAddressSize);
		tData[i].socket = hSocket;
		tData[i].threadID = i;
		
		thID = pthread_create (&threads[i], NULL, tProxyJob, (void *) &tData[i]);
		if (thID) {
			printf ("\nError Creating a thread\nDying...\n");
			return 1;
		}
	}
}



