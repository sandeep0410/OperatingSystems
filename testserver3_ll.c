#include <stdio.h>
#include <string.h>   //strlen
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>   //close
#include <arpa/inet.h>    //close
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros
struct client_data{
	struct client_data *next;
	char *buf;
	int to_delete;
	int con_num;
	int sock_id;
};

#define TRUE   1
#define FALSE  0
#define PORT 5555
#define BUF_SIZE 4096
unsigned int connection_count = 0,active_count = 0;
struct client_data * insert_data(struct client_data * Head);
struct client_data * delete_node(struct client_data * Head);

int main(int argc, char *argv[])
{
	int opt = TRUE;
	int sock, addrlen, new_socket, newsockfd, i, valread, sd;
	int max_sd;
	struct sockaddr_in address;
	struct client_data *HEAD,*blk;

	//set of socket descriptors
	fd_set readfds;

	//create a master socket
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	//set master socket to allow multiple connections , this is just a good habit, it will work without this
	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0)
	{
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}

	//type of socket created
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);

	//bind the socket to localhost port 5555
	if (bind(sock, (struct sockaddr *)&address, sizeof(address))<0)
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}

	//try to specify maximum of 3 pending connections for the master socket
	if (listen(sock, 1000) < 0)
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}

	//accept the incoming connection
	addrlen = sizeof(address);
	HEAD = NULL;
	while (TRUE)
	{
		//clear the socket set
		FD_ZERO(&readfds);

		//add master socket to set
		FD_SET(sock, &readfds);
		max_sd = sock;
		for (blk=HEAD;blk;blk = blk->next)
		{
			//socket descriptor
			sd = blk->sock_id;
			
			//if valid socket descriptor then add to read list
			if (sd > 0){
				FD_SET(sd, &readfds);		
			}

			//highest file descriptor number, need it for the select function
			if (sd > max_sd)
				max_sd = sd;
		}


		//wait for an activity on one of the sockets , timeout is NULL , so wait indefinitely
		newsockfd = select(max_sd + 1, &readfds, NULL, NULL, NULL);

		/*if ((newsockfd < 0) && (errno != EINTR))
		{
			printf("select error, newsockfd = %d,err = %s\n",newsockfd,strerror(errno));
		}*/
		
		
		//If something happened on the master socket , then its an incoming connection
		if (FD_ISSET(sock, &readfds))
		{
			if ((new_socket = accept(sock, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
			{
				perror("accept");
				exit(EXIT_FAILURE);
			}

			connection_count++;
			active_count++;
			//printf("Total connections = %d,Active conections = %d\n",connection_count,active_count);
                  	//printf("connection number = %d, socket id = %d\n",connection_count,new_socket);
		  	//Create a linked list data
			//printf("Calling insert into head\n");
			HEAD = insert_data(HEAD);
			HEAD->sock_id = new_socket;
		}

		//else its some IO operation on some other socket :)
		for(blk=HEAD;blk;blk = blk->next)
		{
			sd = blk->sock_id;
			

			if (FD_ISSET(sd, &readfds))
			{
				
				//Check if it was for closing , and also read the incoming message				
				if ((valread = read(sd, blk->buf, BUF_SIZE)) == 0)
				{
					close(sd);
					blk->to_delete = 1;
					active_count--;
				}
				//Echo back the message that came in
				else
				{
					//set the string terminating NULL byte on the end of the data read
					//buffer[valread] = '\0';
					//	puts(buffer);
					//send(sd , buffer , strlen(buffer) , 0 );
				}
				//printf("Valread = %d, socket id = %d\n",valread,blk->sock_id);
			}
		}
		if(active_count == 0 && connection_count > 0)
		{
			HEAD = delete_node(HEAD);
		}
	}

	return 0;
}
struct client_data * insert_data(struct client_data * Head)
{
	struct client_data * temp;
	temp = (struct client_data *)malloc(sizeof(struct client_data));
	temp->buf = (char *)malloc(sizeof(char) * BUF_SIZE);
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
				//printf("Deleting node %d\n",temp->con_num);
				//connection_count--;
			//	close(temp->sock_id);
				free(temp->buf);
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
			//printf("Deleting node %d\n",temp->con_num);
			//connection_count--;
		//	close(temp->sock_id);			
			free(temp->buf);
			free(temp);
		//	printf("A node deleted\n");
		}
	}
	return Head;	
}
