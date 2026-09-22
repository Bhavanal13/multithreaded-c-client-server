//Author: Bhavana Lokesh
#include "cs_common.h"
#include "cs_client.h"

int main() {
	// create the client socket
	int sock;
	int status = 0;
	struct sockaddr_in server_addr;
	int op_type;
	char buffer[MAX_BUFFER_SIZE];
	do{
		sock = socket(AF_INET, SOCK_STREAM, 0);
		if (sock < 0) {
			perror("socket failed. Error\n");
			status = 1;
			break;
		}
		// connect to the server
		server_addr.sin_family = AF_INET;
		server_addr.sin_port = htons(CONNECTING_PORT);
		server_addr.sin_addr.s_addr = inet_addr(CLIENT_IP);
		if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
			perror("connect failed. Error\n");
			status = 2;
			break;
		}
		// send the operation type to the server
		printf("Enter operation type\n 1 for SELECT\n 2 for UPDATE\n 3 for DELETE\n 4 for INSERTION\n 5 for EXIT\n  ");
		scanf("%d", &op_type);
		cric_info ci = {0};
		switch (op_type) {
			case 1: // select operation
				// receive the table from the server and print it
				if (send(sock, &op_type, sizeof(int), 0) < 0) {
					perror("sending user request failed. Error\n");
					status = 3;
					break;
				}
				memset(buffer, '\0',MAX_BUFFER_SIZE);
				if (recv(sock, buffer, MAX_BUFFER_SIZE, 0) < 0) {
					perror("recieving select status failed. Error");
					status = 4;
					break;
				}
				if(buffer[0] == '\0')
					printf("Table is empty\n");

				print_table(buffer);
				break;

			case 2: // update operation
				// send the id and name to be updated to the server
				if (send(sock, &op_type, sizeof(int), 0) < 0) {
					perror("sending user request failed. Error\n");
					status = 3;
					break;
				}
				printf("Enter id:\n ");
				scanf("%d",&ci.id);
				getchar();
				printf("Enter name:\n ");
				scanf("%[^\n]",ci.name);
				getchar();
				printf("Enter country:\n");
				scanf("%[^\n]",ci.country);
				printf("Enter batting avg:\n ");
				scanf("%f",&ci.batting_avg);
				printf("Enter bowling avg:\n ");
				scanf("%f",&ci.bowling_avg);

				printf("%d, %s, %s, %f, %f\n",ci.id, ci.name, ci.country, ci.batting_avg, ci.bowling_avg);
				sprintf(buffer,"%d|%s|%s|%f|%f| \n", ci.id, ci.name, ci.country, ci.batting_avg, ci.bowling_avg);
				if (send(sock, buffer, MAX_BUFFER_SIZE, 0) < 0) {
					perror("sending update request failed. Error");
					status = 5;
					break;
				}
				memset(buffer,'\0',MAX_BUFFER_SIZE);
				// receive confirmation from the server and print it
				if (recv(sock, buffer, MAX_BUFFER_SIZE, 0) < 0) {
					perror("recieving update status failed. Error");
					status = 6;
					break;
				}
				printf("%s\n", buffer);
				break;

			case 3: // send the id to be deleted to the server
				if (send(sock, &op_type, sizeof(int), 0) < 0) {
					perror("sending user request failed. Error\n");
					status = 3;
					break;
				}
				printf("Enter id: ");
				scanf("%d", &ci.id);
				if (send(sock, &ci.id, sizeof(int), 0) < 0) {
					perror("sending delete request failed. Error");
					status = 7;
					break;
				}
				memset(buffer,'\0',MAX_BUFFER_SIZE);
				// receive confirmation from the server and print it
				if (recv(sock, buffer, MAX_BUFFER_SIZE, 0) < 0){
					perror("recieving delete status failed. Error");
					status = 8;
					break;
				}
				printf("%s\n",buffer);
				break;
			case 4: //Insert Operation
				// send the id to be inserted to the server
				if (send(sock, &op_type, sizeof(int), 0) < 0) {
					perror("sending user request failed. Error\n");
					status = 3;
					break;
				}				
				printf("Enter id:\n ");
				scanf("%d",&ci.id);
				getchar();
				printf("Enter name:\n");
				scanf("%[^\n]",ci.name);
				getchar();
				printf("Enter country:\n");
				scanf("%[^\n]",ci.country);
				printf("Enter batting avg:\n ");
				scanf("%f",&ci.batting_avg);
				printf("Enter bowling avg:\n ");
				scanf("%f",&ci.bowling_avg);

				printf("%d, %s, %s, %f, %f\n",ci.id, ci.name, ci.country, ci.batting_avg, ci.bowling_avg);
				sprintf(buffer,"%d|%s|%s|%f|%f|\n", ci.id, ci.name, ci.country, ci.batting_avg, ci.bowling_avg);

				if (send(sock, buffer,MAX_BUFFER_SIZE, 0) < 0) {
					perror("sending insert request failed. Error");
					status = 9;
					break;
				}
				memset(buffer,'\0',MAX_BUFFER_SIZE);
				if (recv(sock, buffer, MAX_BUFFER_SIZE, 0) < 0){
					perror("recieving insert status failed. Error");
					status = 10;
					break;
				}
				printf("%s\n",buffer);
				break;
			case 5:	printf("client exiting");
                                if (send(sock, &op_type, sizeof(int), 0) < 0) {
					perror("sending user request failed. Error\n");
					status = 3;
					break;
				}
				close(sock);				
				exit(1);		

			default:
				printf("Invalid operation type\n");
				break;
		}

	}while(getchar()!= 5);
	if(status != 0){
		close(sock);
	}
	return 0;

}
