// Marc Stocker
// Kevin Okele
//=================================================
// This is the Marc Branch
//=================================================

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define SERVER_PORT "5432"
#define MAX_LINE 256
#define MAX_PENDING 5

int
main(int argc, char *argv[])
{
	struct addrinfo hints;
	struct addrinfo *rp, *result;
	char buf[MAX_LINE];
	int s, new_s;
	int len;
	char *filename;
	FILE *fp;

	/* Build address data structure */
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_protocol = 0;
	hints.ai_canonname = NULL;
	hints.ai_addr = NULL;
	hints.ai_next = NULL;

	/* Get local address info */
	if ((s = getaddrinfo(NULL, SERVER_PORT, &hints, &result)) != 0 )
	{
		fprintf(stderr, "%s: getaddrinfo: %s\n", argv[0], gai_strerror(s));
		exit(1);
	}

	/* Iterate through the address list and try to perform passive open */
	for (rp = result; rp != NULL; rp = rp->ai_next)
	{
		if ((s = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol)) == -1 )
		{
			continue;
		}

		if (!bind(s, rp->ai_addr, rp->ai_addrlen))
		{
			break;
		}

		close(s);
	}
	if (rp == NULL)
	{
		perror("stream-talk-server: bind");
		exit(1);
	}
	if (listen(s, MAX_PENDING) == -1)
	{
		perror("stream-talk-server: listen");
		close(s);
		exit(1);
	}

	/* Wait for connection, then receive and print text */
	int repeat = 1;
	while(repeat == 1)
	{
		if ((new_s = accept(s, rp->ai_addr, &(rp->ai_addrlen))) < 0)
		{
			perror("stream-talk-server: accept");
			close(s);
			exit(1);
		}
		len = recv(new_s, buf, sizeof(buf), 0);
		if(len > 0)
		{
			// Save filename from client
			filename = buf;
			printf("The file to be transfered: %s\n\n", filename);
			// Open the file
			fp = fopen(filename, "r");
			// ERROR opening file
			if (fp == NULL)
			{
				fprintf(stderr, "Can't open input file '%s'!\n", filename);
				//send error to client - tell them we cannot open/find file
				 char *errorr;
				 errorr = "SERVER ERROR";
				send(new_s, errorr, strlen(errorr)+1, 0);

				//Close connections and exit program
				close(new_s);
				freeaddrinfo(result);
				close(s);
				exit(1);
			}

			printf("========= Printing/Sending file contents =========\n\n");
			// Opened file successfully, send contents of file
			int x;
			while((x = fgetc(fp)) != EOF)
			{
				// printf("%c", x);
				send(new_s, &x, 1, 0);
			}

			printf("\n\n\n--------- Finished Sending --------------\n\n");
			repeat = 0;
		}


		// close connection with client
		close(new_s);
		// close file for reading
		fclose(fp);
	}

	freeaddrinfo(result);
	close(s);

	return 0;
}
