#ifndef __CS_COMMON_H
#define __CS_COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>

#define CONNECTING_PORT 8080
#define MAX_NAME_LEN 50
#define MAX_COUNTRY_LEN 30

struct cricketer{
	int id;
	char name[MAX_NAME_LEN];
	char country[MAX_COUNTRY_LEN];
	float batting_avg;
	float bowling_avg;
};
typedef struct cricketer cric_info;

#define MAX_BUFFER_SIZE 1024

#endif
