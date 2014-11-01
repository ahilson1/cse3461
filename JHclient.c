#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>

#define BUFFER_SIZE 20
#define SERVER "127.0.0.1"

int main(int argc, char *argv[]){

	//checks that enough arguments were given
	if(argc < 3){
		fprintf(stderr, "Not enough arguments given \n");
		return -1;
	}


	//declare and grab all command line arguments
	char address[BUFFER_SIZE];
	int port;
	char file_name[BUFFER_SIZE];

	strcpy(address,argv[1]);
	port = atoi(argv[2]);
	strcpy(file_name, argv[3]);


	//print cmd line args
	printf("%s %d %s \n", address, port, file_name);


	//begin creating socket
	int cli_s;
  struct sockaddr_in cli_addr, serv_addr;  //structure used to hold an internet address

  /*AF_INET = IP address family
  //SOCK_DGRAM = type of datagram service*/
  if((cli_s = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("Cannot create client socket.");
		return 0;
	} else {
		printf("Client socket created.\n");
	}


	/* bind to an arbitrary return address */
	/* because this is the client side, we don't care about the address */
	/* since no application will initiate communication here - it will just send responses */
	/* INADDR_ANY is the IP address and 0 is the socket */
	/* htonl converts a long integer (e.g. address) to a network representation */
	/* htons converts a short integer (e.g. port) to a network representation */
	memset((char *)&cli_addr, 0, sizeof(cli_addr));
	cli_addr.sin_family = AF_INET;
	cli_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	cli_addr.sin_port = htons(0);

	if (bind(cli_s, (struct sockaddr *)&cli_addr, sizeof(cli_addr)) < 0) {
		perror("Bind failed");
		return 0;
	} else {
		printf("Bind succesful.\n");
	}


	//Filling in the servers address info into its' struct serv_addr
	char *my_message = "Test message";  //will be replaced with a file later!
	memset((char*)&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);

	if (inet_aton(SERVER , &serv_addr.sin_addr) == 0)
    {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }


	/* put the host's address into the server address structure */
	// memcpy((void *)&serv_addr.sin_addr, "127.0.0.1", sizeof("127.0.0.1"));

	if(sendto(cli_s, my_message, strlen(my_message), 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
		perror("sendto failed");
		return 0;
	}

	
}