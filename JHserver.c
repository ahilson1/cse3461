#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define BUFSIZE 2048

int main(int argc, char *argv[]){

	int port;
	port = atoi(argv[1]);

	
	struct sockaddr_in serv_addr;      		/* our address */
  struct sockaddr_in remaddr;     			/* remote address */
  socklen_t addrlen = sizeof(remaddr);  /* length of addresses */
  int recvlen;                    			/* # bytes received */
  int serv_s;                         	/* our socket */
  unsigned char buf[BUFSIZE];     			/* receive buffer */

  /* create a UDP socket */
  if((serv_s = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
  	perror("Cannot create server socket\n");
    return 0;
  } else {
  	printf("Server socket created\n");
  }

  memset((char *)&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

  serv_addr.sin_port = htons(port);

  if(bind(serv_s, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
  	perror("Bind failed");
    return 0;
  } else {
    printf("Bind succesful\n");
  }


          
 	for (;;) {
		printf("Waiting on port: %d\n", port);
		recvlen = recvfrom(serv_s, buf, BUFSIZE, 0, (struct sockaddr *)&remaddr, &addrlen);
    printf("Received %d bytes\n", recvlen);
    	if (recvlen > 0) {
      	buf[recvlen] = 0;
       	printf("Received message: \"%s\"\n", buf);
      }
  }

	// bind(serv_s, (struct sockaddr *)&serv_addr, sizeof(cli_addr));






}