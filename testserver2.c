#include <stdio.h>
#include <stdlib.h>

#include <netdb.h>
#include <netinet/in.h>

#include <string.h>
#include <sys/unistd.h>
#include <sys/fcntl.h>
#include <aio.h>
#include<errno.h>
struct client_data{
	struct aiocb *aiocbo_ptr;
	struct client_data *next;
	int to_delete;
	int con_num;
};
static int connection_count = 0;
#define BUF_SIZE 1024
//struct aiocb *aiocbo_ptr[10000];
void executeFunction(int newsockfd);
void callAIOREAD(struct aiocb* aiocbptr, int offset, int sockfd);
struct client_data * insert_data(struct client_data * Head);
struct client_data * delete_node(struct client_data * Head);
int main(int argc, char *argv[]) 
{
	int sockfd, newsockfd, portno, clilen, *new_sock;
  	char *buffer;
  	int i;
	struct sockaddr_in serv_addr, cli_addr;
	struct client_data *HEAD,*blk;
	/* First call to socket() function */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	fcntl(sockfd, F_SETFL, O_NONBLOCK);
	if (sockfd < 0) {
		perror("ERROR opening socket");
		exit(1);
	}

	/* Initialize socket structure */
	bzero((char *)&serv_addr, sizeof(serv_addr));
	portno = 5555;

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
	HEAD = NULL;
	/* Accept actual connection from the client */
	while (1) 
	{
		newsockfd=-1;
		newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);

		if (newsockfd>0 )
		{ 
                  	//printf("connection number %d\n",i);
			connection_count++;
                  	printf("connection number %d\n",connection_count);
		  	//Create a linked list data
			//printf("Calling insert into head\n");
			HEAD = insert_data(HEAD);
			//printf("Inserted into data list\n");
			callAIOREAD(HEAD->aiocbo_ptr, 0, newsockfd);
		}
		for(blk=HEAD;blk;blk = blk->next)
 		{
        		if(aio_error(blk->aiocbo_ptr) == 0)
       			 {
				if(strlen((char *)blk->aiocbo_ptr->aio_buf)==BUF_SIZE)
				{
          				callAIOREAD(blk->aiocbo_ptr, blk->aiocbo_ptr->aio_offset+BUF_SIZE,blk->aiocbo_ptr->aio_fildes );
				}
				else
				{
					//transfer complete delete node
					blk->to_delete = 1;
					close(newsockfd);
				}
        		}
		}
		HEAD = delete_node(HEAD);
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
struct client_data * insert_data(struct client_data * Head)
{
	struct client_data * temp;
	temp = (struct client_data *)malloc(sizeof(struct client_data));
	temp->aiocbo_ptr = (struct aiocb *)malloc(sizeof(struct aiocb));
	temp->next = Head;
	temp->to_delete = 0;
	temp->con_num = connection_count;
	return temp;	
}
struct client_data * delete_node(struct client_data * Head)
{
	struct client_data *temp,*curr_data;
	if(Head !=NULL)
	{
		curr_data = Head;
		while(curr_data->next != NULL)
		{
			if(curr_data->next->to_delete == 1)
			{
				
				temp = curr_data->next;
				curr_data->next = curr_data->next->next;
				printf("Deleting node %d\n",curr_data->con_num);
				connection_count--;
				free(temp->aiocbo_ptr);
				free(temp);
				//printf("A node deleted\n");
				
			}
			else
			{
				curr_data = curr_data->next;
			}
		}	
		
		if(Head->to_delete == 1)
		{
			temp = Head;
			Head = Head->next;
			printf("Deleting node %d\n",curr_data->con_num);
			connection_count--;
			free(temp->aiocbo_ptr);
			free(temp);
		//	printf("A node deleted\n");
		}
	}
	return Head;	
}
