#ifndef __CS_SERVER_H
#define __CS_SERVER_H

#include <mysql/mysql.h>
#include"cs_common.h"

#define SERVER "localhost"
#define ADMIN "Trivikram"
#define PASSWORD "sqldb"
#define DATABASE_NAME "cricscore_db" 

char query[MAX_BUFFER_SIZE];
char buffer[MAX_BUFFER_SIZE];
int buffer_size = 0;

void string_operation(char *buffer,cric_info *data);

#endif
