#ifndef _SASQL_H_
#define _SASQL_H_

#include "version.h"
#ifdef _SASQL
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#include <strings.h>
#include <time.h>
#include <sys/time.h>

#include <errno.h>
#include <string.h>

void sasql_close( void );
int sasql_save_nm( int idx, char *acc, char *data );//左右人物, 帐号, 资料
int sasql_save_opt( int idx, char *acc, char *data );
int sasql_save_int_info( int idx, char *acc, char *data );
int sasql_save_char_info( int idx, char *acc, char *data );

BOOL sasql_init( void );
BOOL sasql_query(char *nm, char *pas);
BOOL sasql_update(char *nm, char *path);
BOOL sasql_logindate(char *nm);
BOOL sasql_online(char *nm,char *lnm,int online);
BOOL sasql_register(char *nm, char *pas);
#endif

#endif
