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
// Client

int main(int argc, char *argv[])
{
	if (argc != 3){
		cerr << "Usage: ip_address port" << endl;
		exit(0);
	}

	// get IP address and port
	char *serverIP = argv[1];
	int port = atoi(argv[2]);

	char message[1500];
	
	// create socket
	struct hostent* host = gethostbyname(serverIP);
	sockaddr_in sendSockAddr;
	bzero((char*)&sendSockAddr, sizeof(sendSockAddr));
	sendSockAddr.sin_family = AF_INET;
	sendSockAddr.sin_addr.s_addr = inet_addr(inet_ntoa(*(struct in_addr*)*host->h_addr_list));
	sendSockAddr.sin_port = htons(port);

	int clientSD = socket(AF_INET, SOCK_STREAM, 0);

	int status = connect(clientSD, (sockaddr*) &sendSockAddr, sizeof(sendSockAddr));
	
	if (status < 0){
		cout << "Cannot connect to socket" << endl;
		exit(0);
	}
	
	cout << "Connected to server" << endl;
	int bytesRead, bytesWritten = 0;
	struct timeval start1, end1;
	gettimeofday(&start1, NULL);

	while(1){
		cout << ">";
		string data;
		getline(cin, data);
		memset(&message, 0, sizeof(message));
		strcpy(message, data.c_str());
		if (data == "exit"){
			send(clientSD, (char*)&message, strlen(message), 0);
			break;
		}
		
		bytesWritten += send(clientSD, (char*)&message, strlen(message), 0);
		cout << "Waiting for server response..." << endl;
		memset(&message, 0, sizeof(message));
		bytesRead += recv(clientSD, (char*)&message, sizeof(message), 0);
		if (!strcmp(message, "exit")){
			cout << "Server has quit" << endl;
			break;
		}
		cout << "Server: " << message << endl;
	}
	gettimeofday(&end1, NULL);
	close(clientSD);
	cout << "Connection closed" << endl;
	return 0;
}
