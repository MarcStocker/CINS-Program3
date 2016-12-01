/* Kevin Okeke
*	 Marc Stocker
*CSCI 446
*Fall 2016
*/

#include <iostream>
using namespace std;
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <cstring>
#include <errno.h>
#include <stdio.h>

#define SERVER_NAME "www.ecst.csuchico.edu"
#define SERVER_PORT "80"
#define REQUEST "GET /~ehamouda/file.html HTTP/1.0\n\n"

ssize_t readchunck( int sockfd, char *buf, size_t len, char *keyword );


int main (int argc, char *argv[])
{

	if(argc > 3  || argc < 3)
	{
		cout << "ERROR:  Incorrect number of commandline arguments" << endl;
		cout << "usage: "<<argv[0]<<" 'chunck size' 'string'" <<endl;
		return 1;
	}
	char *keyword = argv[2];
	signed int lownum = strlen(keyword);
	if (atoi(argv[1]) > 1000)
	{
		cout << "ERROR: chunk size too large" << endl;
		return 1;
	}
	else if(atoi(argv[1]) < lownum)
	{
		cout << "ERROR: chunk size too small" << endl;
		return 1;
	}
	size_t csize;
	csize = atoi(argv[1]);

	// Connect to the Server
	int sock;  // get return value of socket function
	char buffer[csize]; // to hold the data
	struct addrinfo hints;
	struct addrinfo *res;
	int status;


	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;


	if ((status = getaddrinfo(SERVER_NAME, SERVER_PORT, &hints, &res)) != 0)
	{
		printf("Error: getaddrinfo %s", strerror(errno));
		//cerr << "getaddrinfo: " << strerr(errno) << endl;
		return 1;
	}
	if ((sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1 )
	{
		printf("ERROR could not connect to socket: socket %s", strerror(errno));
		return 1;
	}
	if (connect(sock, res->ai_addr, res->ai_addrlen) == -1)
	{
		printf("ERROR could not connect: connect %s", strerror(errno));
		close(sock);
		return 1;
	}
	// send messages
	int len;
	len = strlen(REQUEST);
	send(sock, REQUEST, len, 0);

	readchunck(sock, buffer, csize, keyword);
	close(sock);


	return 0;
}

ssize_t readchunck( int sockfd, char *buf, size_t len, char *keyword )
{
	// cout << "==============================="<<endl;
	// cout << "==    Reading File Chunks    =="<<endl;
	// cout << "==      Buffer Size of       =="<<endl;
	// if(len < 10)
	// 	cout << "==             "<< len <<"             =="<<endl;
	// else if(len < 100)
	// 	cout << "==            "<< len <<"             =="<<endl;
	// else if(len > 99)
	// 	cout << "==           "<< len <<"             =="<<endl;
	// cout << "==============================="<<endl<<endl;

	//int bufnum = 1; // DEBUG VARIABLE
	int count = 0;
	char *pch;

	while( (recv(sockfd, buf, len, 0)) > 0)
	{
		// DEBUG OUTPUT
		//cout << "===========  Printing Buffer #"<< bufnum<<" ===========\n";
		//cout << buf << endl;
		//cout << "==============  End buffer #"<< bufnum<<" ===============\n\n";
		//bufnum++;

		pch = strstr(buf, keyword);
		while (pch != NULL)
		{
			count ++;
			//DEBUG CODE
				//cout << "pch----------------\n";
				//cout << pch << endl;
				//cout << "buf----------------\n";
				//cout << buf << endl;

			pch = strstr(pch+strlen(keyword), keyword);
		}


	}
	// FINAL OUTPUT - CUSTOM FORMAT
		// cout <<"==============================="<<endl;
		// cout << "==     Read File Chunks      =="<<endl;
		// cout << "==      Buffer Size of       =="<<endl;
		// if(len < 10)
		// cout << "==             "<< len <<"             =="<<endl;
		// else if(len < 100)
		// cout << "==            "<< len <<"             =="<<endl;
		// else if(len > 99)
		// cout << "==           "<< len <<"             =="<<endl;
		// cout << "==============================="<<endl;
		// cout << "==   We found a total of     =="<<endl;
		// cout << "==           "<<count<<"              =="<<endl;
		// cout << "==  instances of the string  =="<< endl;
		// cout << "==           "<<keyword<<"             =="<< endl;
		// cout << "==============================="<<endl;
	//FINAL OUTPUT - REQUIRED FORMAT
	cout << "Number of "<< keyword <<" instances: " << count << endl;
	return len;
}
