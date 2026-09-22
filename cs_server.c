//Author: Bhavana Lokesh
#include "cs_common.h"
#include "cs_server.h"


// string operations done here

void string_operation(char *buffer,cric_info *data){
	int len = strlen(buffer);
	static int field = 1;
	int j = 0,i = 0;
	char temp[MAX_NAME_LEN];
	while(buffer[i] != '\0')
	{

		if(buffer[i] != '|'){
			temp[j] = buffer[i];
			j++;

		}
		else {
			temp [i] = '\0';						
			if(field == 1){
				data->id = atoi(temp);
				j = 0;
				memset(temp,'\0',50);
			}
			else if(field == 2){
				strcpy(data->name,temp);
				printf("%s name",data->name);
				j = 0;
				memset(temp,'\0',50);

			}
			else if(field == 3){
				strcpy(data->country,temp);
				printf("%s country",data->country);
				j = 0;
				memset(temp,'\0',50);
			}

			else if(field == 4){
				data->batting_avg = atof(temp);
				j = 0;
				memset(temp,'\0',50);
			}
			else if(field == 5){
				data->bowling_avg = atof(temp);
				j = 0;
				memset(temp,'\0',50);
				break;
			}
			field++;
		}
		i++;				

	}
}


// Send to server thread starts from here

void *send_to_client(void *arg) 
{
	int *psock =(int*)arg;
	int sock = *psock;
	MYSQL *conn = NULL;
	char buffer[MAX_BUFFER_SIZE];
	int bytes_sent;
	cric_info cis = {0};
	MYSQL_RES *res = NULL;
	MYSQL_ROW row;
	int th_status = 0;
	do{
		// receive the operation type from the client
		int op_type = 0;
		if (recv(sock, &op_type, sizeof(int), 0) < 0) {
			perror("recieving request failed. Error");
			th_status = 1;
			break;
		}
		printf("cmd recieved %d\n", op_type);
		switch (op_type) {
			case 1: // select operation
				// query the database
				conn = mysql_init(NULL);
				if (!mysql_real_connect(conn, SERVER, ADMIN, PASSWORD, DATABASE_NAME, 0, NULL, 0)) {
					fprintf(stderr, "%s\n", mysql_error(conn));
					th_status = 2;
					break;
				}

				snprintf(query, MAX_BUFFER_SIZE, "SELECT * FROM cricketers");
				if (mysql_query(conn, query)) {
					fprintf(stderr, "%s\n", mysql_error(conn));
					th_status = 3;
					break;
				}

				res = mysql_store_result(conn);
				memset(buffer,'\0',MAX_BUFFER_SIZE);
				while ((row = mysql_fetch_row(res))) {
					for (int i = 0; i < mysql_num_fields(res); i++) {
						buffer_size += snprintf(buffer + buffer_size, MAX_BUFFER_SIZE - buffer_size, "%s\t", row[i] ? row[i] : "NULL");
					}
					buffer_size += snprintf(buffer + buffer_size, MAX_BUFFER_SIZE - buffer_size, "\n");
					if (buffer_size > MAX_BUFFER_SIZE - 100) {
						send(sock, buffer, buffer_size, 0);
						buffer_size = 0;
					}
				}
				if(buffer[0] == '\0')
					printf("Table is empty\n");
				
				send(sock, buffer, buffer_size, 0);

				if(res != NULL)
					mysql_free_result(res);
				if(conn != NULL){
					mysql_close(conn);
					mysql_library_end();
				}				
				break;

			case 2: // update operation
				// receive the id and name, country,batting_avg ,bowling_avg from the client

				conn = mysql_init(NULL);

				if (!mysql_real_connect(conn, SERVER, ADMIN, PASSWORD, DATABASE_NAME, 0, NULL, 0)) {
					fprintf(stderr, "%s\n", mysql_error(conn));
					th_status = 4;
					break;
				}
				// Function calling

				memset(buffer,'\0',MAX_BUFFER_SIZE);
				if (recv(sock, buffer, MAX_BUFFER_SIZE, 0) < 0) {
					perror("recv failed. Error");
					th_status = 5;
					break;
				}
				string_operation(buffer,&cis);

				// update the database
				sprintf(buffer, "UPDATE cricketers SET name = '%s' ,country='%s', batting_avg ='%f', bowling_avg= '%f' WHERE id=%d",cis.name, cis.country,cis.batting_avg,cis.bowling_avg, cis.id);

				printf(" the query is : %s\n", buffer);
				if (mysql_query(conn, buffer)) {
					fprintf(stderr, "%s\n", mysql_error(conn));
					th_status = 6;
					break;
				}

				// send confirmation to the client
				char confirmation[] = "Update successful\n";
				if (send(sock, confirmation, strlen(confirmation), 0) < 0) {
					perror("send failed. Error");
					th_status = 7;
					break;
				}
				printf("Updation is done successfully!!\n");
				if(conn != NULL){
					mysql_close(conn);
					mysql_library_end();
				}

				break;

			case 3: // delete operation
				// receive the id from the client
				if (recv(sock, &cis.id, sizeof(int), 0) < 0) {
					perror("recv failed. Error");
					th_status = 8;
					break;
				}

				conn = mysql_init(NULL);
				if (!mysql_real_connect(conn, SERVER, ADMIN, PASSWORD, DATABASE_NAME, 0, NULL, 0)) {
					fprintf(stderr, "%s\n", mysql_error(conn));
					th_status = 9;
					break;
				}
				// delete the record from the database
				sprintf(buffer, "DELETE FROM cricketers WHERE id=%d", cis.id);
				if (mysql_query(conn, buffer)) {
					fprintf(stderr, "%s\n", mysql_error(conn));
					th_status = 10;
					break;
				}

				// send confirmation to the client
				strcpy(confirmation, "Delete successful\n");
				if (send(sock, confirmation, strlen(confirmation), 0) < 0) {
					perror("send failed. Error");
					th_status = 11;
					break;
				}
				printf("Deletion is done successfully!!\n");
				if(conn != NULL){
					mysql_close(conn);
					mysql_library_end();
				}
				break;

			case 4:// INSERT operation
				// receive the id from the client


				conn = mysql_init(NULL);
				if (!mysql_real_connect(conn, SERVER, ADMIN, PASSWORD, DATABASE_NAME, 0, NULL, 0)) {
					fprintf(stderr, "%s\n", mysql_error(conn));
					th_status = 12;
					break;
				}
				memset(buffer,'\0',MAX_BUFFER_SIZE);
				if (recv(sock, buffer, MAX_BUFFER_SIZE, 0) < 0) {
					perror("recv failed. Error");
					return NULL;
				}
				printf("%s\n",buffer);
				// Function calling
				string_operation( buffer,&cis);

				sprintf(buffer, "INSERT INTO cricketers(id,name, country, batting_avg, bowling_avg) VALUES (%d,'%s','%s', %.2f, %.2f)", cis.id, cis.name, cis.country, cis.batting_avg, cis.bowling_avg);

				printf(" \n the query is : %s\n", buffer);

				if (mysql_query(conn,buffer)){
					fprintf(stderr, "%s\n", mysql_error(conn));
					th_status = 13;
					break;
				}

				// send confirmation to the client
				strcpy(buffer, "Insertion successful\n");
				if (send(sock, confirmation, strlen(confirmation), 0) < 0) {
					perror("send failed. Error");
					th_status = 14;
					break;
				}
				if(conn != NULL){
					mysql_close(conn);
					mysql_library_end();
				}
				printf("Insertion is done successfully!!\n");
				break;
			case 5:printf("Bye\n");
			       break;
			default:
			       fprintf(stderr, "Invalid operation type\n");
			       break;
		}
	}while(0);
	close(sock);
	//pthread_exit("Bye\n");
	return NULL;
}


// main function starts here

int main() {
	int srv_status = 0;
	struct sockaddr_in server_addr;
	int server_sock;
	MYSQL *conn = NULL;
	do{
		// connect to the database
		conn = mysql_init(NULL);
		if (conn == NULL) {
			fprintf(stderr, "%s\n", mysql_error(conn));
			srv_status = 1;
			break;
		}
		if (mysql_real_connect(conn,SERVER, ADMIN, PASSWORD, NULL, 0, NULL, 0) == NULL) {
			fprintf(stderr, "%s\n", mysql_error(conn));
			srv_status = 2;
			break;
		}
		printf("connected to database\n");
		if (mysql_query(conn, "CREATE DATABASE IF NOT EXISTS cricscore_db") != 0) {
			printf( "Error: %s\n", mysql_error(conn));
			srv_status = 3;
			break;
		}
		printf("database created!!\n");
		if (mysql_query(conn, "USE cricscore_db") != 0) {
			printf("Error: %s\n", mysql_error(conn));
			srv_status = 4;
			break;
		}
		if (mysql_query(conn,"CREATE TABLE IF NOT EXISTS cricketers(id INT PRIMARY KEY AUTO_INCREMENT, name VARCHAR(50), country VARCHAR(50), batting_avg FLOAT, bowling_avg FLOAT)") != 0) {
			printf("Error: %s\n", mysql_error(conn));
			srv_status = 5;
			break;
		}
		if(srv_status == 0) {
			mysql_close(conn);
			mysql_library_end();
		}

		// create the server socket
		int server_sock = socket(AF_INET, SOCK_STREAM, 0);
		if (server_sock < 0) {
			perror("socket failed. Error");
			srv_status = 6;
			break;
		}
		// Set socket options
		int opt=1;
		if (setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
			perror("setsockopt failed");
			srv_status = 7;
			break;
		}

		// bind the server socket to a port

		server_addr.sin_family = AF_INET;
		server_addr.sin_port = htons(CONNECTING_PORT);
		server_addr.sin_addr.s_addr = INADDR_ANY;

		if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
			perror("bind failed. Error");
			srv_status = 8;
			break;
		}

		// start listening for incoming connections
		if (listen(server_sock, 5) < 0) {
			perror("listen failed. Error");
			srv_status = 9;
			break;
		}

		// accept incoming connections and handle them in separate threads
		while (1) {
			struct sockaddr_in client_addr;
			int client_sock;
			pthread_t thread;
			socklen_t client_addr_len = sizeof(client_addr);

			// accept a new connection
			client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_addr_len);
			if (client_sock < 0) {
				perror("accept failed. Error");
				srv_status = 10;
				break;
			}
			printf("Client Connected\n");
			// handle the connection in a separate thread
			if (pthread_create(&thread, NULL, send_to_client, (void *)&client_sock) < 0) {
				perror("pthread_create failed. Error");
				srv_status = 11;
				break;
			}
			//pthread_join(thread,NULL);
		}


	}while(0);
	printf("server status %d",srv_status);
	if(srv_status >0 && srv_status < 6){
		mysql_close(conn);
		mysql_library_end();
	}
	if(srv_status > 6)
		close(server_sock);

	return 0;
}
