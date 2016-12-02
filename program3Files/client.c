// Marc Stocker
// Kevin Okele
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
	int s;
	int len;

	if (argc==4)
	{
		host = argv[1];
		host_port= argv[2];
		filename = argv[3];
		len = strlen(filename);
	}
	else
	{
		fprintf(stderr, "usage: %s host\n", argv[0]);
		exit(1);
	}

	printf("\nConnecting to server with following information...\nHost: %s\nHost Port: %s\nFilename: %s\n\n\n", host, host_port, filename);

	/* Translate host name into peer's IP address */
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = 0;
	hints.ai_protocol = 0;

	if ((s = getaddrinfo(host, SERVER_PORT, &hints, &result)) != 0 )
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
		perror("stream-talk-client: connect");
		exit(1);
	}
	freeaddrinfo(result);

	// Send Filename to Server
	printf("Filename to be sent: %s\nLen of file: %i\n", filename, len);
	send(s, filename, len, 0);

	// Close the send socket
	close(s);

	return 0;
}
