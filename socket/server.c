#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

void job_handler(int);
void upper_string(char s[]);

// error when calling system calls
void error(const char *msg) {
	perror(msg);
	exit(1);
}

int main(int argc, char *argv[]) {
	int sockfd, comingsockfd, portno;
	socklen_t clientlen;
	struct sockaddr_in serv_addr, cli_addr;

	if (argc < 2) {
		fprintf(stderr, "ERROR\tno port provided\n");
		exit(1);
	}

	// create new socket in Internet domain, stream type
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		error("ERROR\topening socket");
	}

	bzero((char *) &serv_addr, sizeof(serv_addr));
	portno = atoi(argv[1]);

	// assign values to serv_addr struct
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno); // network byte order

	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		error("ERROR\t on binding");
	}

	listen(sockfd, 5);
	
	clientlen = sizeof(cli_addr);

	while(1) {
		comingsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clientlen);
		if (comingsockfd < 0) {
			error("ERROR\t on accept");
		}
		int pid = fork();
		if (pid < 0) {
			error("ERROR\t on fork");
		}
		if (pid == 0) { // child process
			close(sockfd);
			job_handler(comingsockfd);
			exit(0);
		} else { // parent process
			close(comingsockfd);
		}
	}

	close(sockfd);
	return 0;
}

// convert message to a string with all letters capitalized
void job_handler(int sock) {
	int n;
	char buffer[256], remsg[256];
	bzero(buffer, 256);
	bzero(remsg, 256);

	n = read(sock, buffer, 256);
	if (n < 0) {
		error("ERROR\treading from socket");
	}
	printf("Got a message: %s\n", buffer);

	strcpy(remsg, buffer);
	upper_string(remsg);

	n = write(sock, remsg, strlen(remsg));
	if (n < 0) {
		error("ERROR\twriting to socket");
	}
}

void upper_string(char s[]) {
   int c = 0;
 
   while (s[c] != '\0') {
      if (s[c] >= 'a' && s[c] <= 'z') {
         s[c] = s[c] - 32;
      }
      c++;
   }
}