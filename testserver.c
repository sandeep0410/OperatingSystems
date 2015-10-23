#include <stdio.h>
#include <stdlib.h>

#include <netdb.h>
#include <netinet/in.h>

#include <string.h>
#include <sys/unistd.h>
#include <sys/fcntl.h>

void executeFunction(int newsockfd);
int main(int argc, char *argv[]) {
	int sockfd, newsockfd, portno, clilen, *new_sock;

	struct sockaddr_in serv_addr, cli_addr;

	/* First call to socket() function */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
fcntl(sockfd, F_SETFL, O_NONBLOCK);
	if (sockfd < 0) {
		perror("ERROR opening socket");
		exit(1);
	}

	/* Initialize socket structure */
	bzero((char *)&serv_addr, sizeof(serv_addr));
	portno = 5001;

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);

	/* Now bind the host address using bind() call.*/
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		perror("ERROR on binding");
		exit(1);
	}

	listen(sockfd, 5);
	clilen = sizeof(cli_addr);

	/* Accept actual connection from the client */
	while (1) {
		newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);

		pthread_t t;


		if (newsockfd>0 && pthread_create(&t, NULL, &executeFunction, newsockfd) < 0)
		{
			perror("could not create thread");
			return 1;
		}
	}

	return 0;
}

void executeFunction(int newsockfd) {
	char buffer[1024];
	int  n;
	if (newsockfd < 0) {
		perror("ERROR on accept");
		exit(1);
	}


	bzero(buffer, 1024);
      while( (n = recv(newsockfd , buffer ,  1024, 0))>0)
      {

      }
	if (n < 0) {
		perror("ERROR reading from socket");
		exit(1);
	}

	printf("Here is the message size: %d\n", n);

}

