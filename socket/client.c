#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

// error when calling system calls
void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[]) {
	int sockfd, portno, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	char buffer[256];

	if (argc < 3) {
		fprintf(stderr, "ERROR\t [usage] %s hostname port", argv[0]);
		exit(1);
	}

	portno = atoi(argv[2]);
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		error("ERROR\t opening socket");
	}

	server = gethostbyname(argv[1]);
	if (server == NULL) {
		fprintf(stderr, "ERROR\t no such host\n");
		exit(0);
	}

	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *) server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(portno);

	if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		error("ERROR\t connecting");
	}
	printf("Enter the message: ");
	bzero(buffer, 256);
	fgets(buffer, 256, stdin);
	n = write(sockfd, buffer, strlen(buffer));
	if (n < 0) {
		error("ERROR\t writing to socket");
	}
	bzero(buffer, 256);
	n = read(sockfd, buffer, 256);
	if (n < 0) {
		error("ERROR\t reading from socket");
	}
	printf("%s\n", buffer);
	close(sockfd);

	return 0;

}
