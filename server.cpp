#include <iostream>
#include <string>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <sys/uio.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <fstream>

using namespace std;

//Server
int main(int argc, char *argv[])
{
	if (argc != 2){
		cerr << "Usage: port" << endl;
		exit(0);
	}
	
	// get port number
	int port = atoi(argv[1]);
	char message[1500];

	// set up socket
	sockaddr_in serverAddr;
	bzero((char*)&serverAddr, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddr.sin_port = htons(port);

	int serverSD = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSD < 0){
		cerr << "Cannot establish server socket" << endl;
		exit(0);
	}

	// bind socket
	int bindStat = bind(serverSD, (struct sockaddr*) &serverAddr, sizeof(serverAddr));
	if (bindStat < 0){
		cerr << "Cannot bind socket to local address" << endl;
		exit(0);
	}

	cout << "Searching for client to connect to..." << endl;

	// listen for up to 10 requests
	listen(serverSD, 10);
	// receive request from client
	sockaddr_in newSockAddr;
	socklen_t newSockAddrSize = sizeof(newSockAddr);

	int newSD = accept(serverSD, (sockaddr *)&newSockAddr, &newSockAddrSize);
	if (newSD < 0){
		cerr << "Cannot accept request from client" << endl;
		exit(1);
	}

	cout << "Connected to client" << endl;
	struct timeval start1, end1;
	gettimeofday(&start1, NULL);
	int bytesRead, bytesWritten = 0;

	while(1){
		//receive message from client
		cout << "Waiting for client response..." << endl;
		memset(&message, 0, sizeof(message));
		bytesRead += recv(newSD, (char*)&message, sizeof(message), 0);

		if (!strcmp(message, "exit")){
			cout << "Client has quit" << endl;
			break;
		}
		cout << "Client: " << message << endl;
		cout << ">";
		string data;
		getline(cin, data);
		memset(&message, 0, sizeof(message));
		strcpy(message, data.c_str());
		if (data == "exit"){
			send(newSD, (char*)&message, strlen(message), 0);
			break;
		}
		bytesWritten += send(newSD, (char*)&message, strlen(message), 0);
	}

	gettimeofday(&end1, NULL);
	close(newSD);
	close(serverSD);
	cout << "Connection closed" << endl;
	return 0;
}
