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
	char buf[MAX_LINE];
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

	printf("Filename to be sent: %s\nLen of file: %i\n", filename, len);
	// Send Filename to Server
	n = send(s, filename, len, 0);
	if(n < 0)
		perror("ERROR writing to socket");


	// Receive the file
	//-------------------------------------------------------------

	printf("\n\n\n========= RECEIVING FILE FROM SERVER =============\n\n");
	// Open file for writing
	of = fopen(filename, "w");
	if (of == NULL){
		fprintf(stderr, "Can't open file for writing\n");
		close(s);
		exit(1);
	}
	// Continue reading file while more information exists
	// n = recv(s, buf, len, 0);
	// if (n < 0){
	// 	perror("ERROR reading from server");
	// 	printf("SERVER ERROR reading from server\n");
	// }
	// printf("printing buf: %s\n", buf);
	int x;
	while( recv(s, &x, 1, 0) > 0)
	{
		// printf("printing buf: %s.", buf);
		fprintf(of, "%c", x);
	}




	// Close the send socket
	close(s);
	// Close the file for writing
	fclose(of);

	return 0;
}
