// Marc Stocker
// Kevin Okeke
//=================================================
// This is the Master Branch
//=================================================

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

#define SERVER_PORT "5432"
#define MAX_LINE 256

int main(int argc, char *argv[])
{
	struct addrinfo hints;
	struct addrinfo *rp, *result;
	char *host;
	char *host_port;
	char *filename;
	int s, n;
	int len;
	FILE *of;

	if (argc==4)
	{
		host = argv[1];
		host_port= argv[2];
		filename = argv[3];
		len = strlen(filename)+1;
	}
	else
	{
		fprintf(stderr, "usage: %s host port# fileToTransfer\n", argv[0]);
		exit(1);
	}

	/* Translate host name into peer's IP address */
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = 0;
	hints.ai_protocol = 0;

	if ((s = getaddrinfo(host, host_port, &hints, &result)) != 0 )
	{
		fprintf(stderr, "%s: getaddrinfo: %s\n", argv[0], gai_strerror(s));
		exit(1);
	}

	/* Iterate through the address list and try to connect */
	for (rp = result; rp != NULL; rp = rp->ai_next)
	{
		if ((s = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol)) == -1 )
		{
			continue;
		}

		if (connect(s, rp->ai_addr, rp->ai_addrlen) != -1)
		{
			break;
		}

		close(s);
	}
	if (rp == NULL)
	{
		fprintf(stderr, "Client Error: Unable to connect to host '%s'\n", host);
		exit(1);
	}
	freeaddrinfo(result);

	// Send Filename to download to Server
	n = send(s, filename, len, 0);
	if(n < 0) // if sending fails
		fprintf(stderr, "CLIENT ERROR writing to socket");


	// Receive the file from the server
	//-------------------------------------------------------------
	// Open file for writing
	of = fopen(filename, "w");
	if (of == NULL){
		fprintf(stderr, "CLIENT ERROR Can't open file for copying\n"); // Print to stderr
		close(s);
		exit(1);
	}

	// While file has not trasnferred fully, continue reading/transferring
	int x;
	while( recv(s, &x, 1, 0) > 0)
	{
		// Save contents of recv to file in 'of'
		fprintf(of, "%c", x);
	}
	// Finished transferring file
	// Close file writing stream
	fclose(of);

	// Close the send socket
	close(s);

	return 0;
}
