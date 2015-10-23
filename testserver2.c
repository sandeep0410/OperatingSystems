#include <stdio.h>
#include <stdlib.h>

#include <netdb.h>
#include <netinet/in.h>

#include <string.h>
#include <sys/unistd.h>
#include <sys/fcntl.h>
#include <aio.h>
//#include<aiocb.h>
#include<errno.h>
/*typedef struct listen{
	struct aiocbo *aiocbo_ptr;
	struct listen *next;
};*/
#define BUF_SIZE 32767
struct aiocb *aiocbo_ptr[10000];
void executeFunction(int newsockfd);
void callAIOREAD(struct aiocb* aiocbptr, int offset, int sockfd);
int main(int argc, char *argv[]) {
	int sockfd, newsockfd, portno, clilen, *new_sock;
  char *buffer;
  int i,j;
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

	listen(sockfd, 10000);
	clilen = sizeof(cli_addr);
  i =-1;
	int k=-1;
	/* Accept actual connection from the client */
	while (1) {
		newsockfd=-1;
		if(k<1019)
			newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);

		//pthread_t t;


		if (newsockfd>0 )
		{ i++;
			k++;
                  printf("connection number %d\n",i);
			aiocbo_ptr[i] = (struct aiocb*)malloc(sizeof(struct aiocb));
		  callAIOREAD((struct aiocb*)aiocbo_ptr[i], 0, newsockfd);
	}
			for(j=0;j<=i;j++)
      {
        if(aiocbo_ptr[j] == 0)
        {
          continue;
        }
        if(aio_error(aiocbo_ptr[j]) == EINPROGRESS)
      	{
						//printf("buffer is: %s and length is %d\n ",aiocbo_ptr[j]->aio_buf, strlen((char *)aiocbo_ptr[j]->aio_buf));
      		//printf("Reading client %d\n",j);
      	}
        else if(aio_error(aiocbo_ptr[j]) == 0)
        {
					//printf("buffer is: %s and length is %d\n ",aiocbo_ptr[j]->aio_buf, strlen((char *)aiocbo_ptr[j]->aio_buf));
					if(strlen((char *)aiocbo_ptr[j]->aio_buf)==BUF_SIZE)
					{
          callAIOREAD((struct aiocb*)aiocbo_ptr[j], (aiocbo_ptr[j]->aio_offset)+BUF_SIZE,aiocbo_ptr[j]->aio_fildes );
						//printf("Increasing the buffer for %d\n",j);
				}else{
					aiocbo_ptr[j] = 0;
					//printf("Deleting the memory for %d\n",j);
          //free(&aiocbo_ptr[i]->aio_buf);
					close(newsockfd);
					k--;
          free((struct aiocb*)aiocbo_ptr[j]);
					}
        }
        /*else if(aio_error(aiocbo_ptr[j]) == -1)
        {
          //remove from array
          aiocbo_ptr[j] = 0;
					printf("Deleting the memory for %d\n",j);
          free(&aiocbo_ptr[i]->aio_buf);
          free(&aiocbo_ptr[i]);
        }*/

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

	//printf("Here is the message size: %d\n", n);

}

void callAIOREAD(struct aiocb* aiocbptr, int offset, int sockfd){
	memset(aiocbptr, 0, sizeof(struct aiocb));
	aiocbptr->aio_buf = (char *)malloc(sizeof(char) * BUF_SIZE);
	aiocbptr->aio_nbytes = BUF_SIZE;
	aiocbptr->aio_fildes = sockfd;
	aiocbptr->aio_offset = offset;

if (aio_read(aiocbptr) == -1)
	{
			printf("Unable to start read");
	}
}

