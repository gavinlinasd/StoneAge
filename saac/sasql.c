#define _SASQL_C_

#include "version.h"

#ifdef _SASQL1//������

#include "main.h"
#include "util.h"
#include "mail.h"
#include "db.h"
#include "saacproto_util.h"
#include "saacproto_serv.h"
#ifdef _UNIVERSE_CHATROOM
#include "chatroom.h"
#endif
// CoolFish: Family 2001/5/9
#include "acfamily.h"

#ifdef _DEATH_CONTEND
#include "deathcontend.h"
#endif

#include <signal.h>
#include <sys/types.h>
#include <time.h>
#include <sys/time.h>
#include <errno.h>
#include <sys/wait.h>
#include <getopt.h>
#include <stdio.h>
#include <malloc.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/tcp.h>

#include "saacproto_work.h"
#ifdef _OACSTRUCT_TCP
#include "saacproto_oac.h"
#endif
#ifdef _PAUCTION_MAN
#include "auction.h"
#endif
#include "lock.h"
#define BACKLOGNUM 5


#ifdef _FIX_WORKS
#include "saacproto_work.h"
int worksockfd;
#endif

#ifdef _LOCK_SERVER
#include "saacproto_lserver.h"
#endif

#ifdef _SEND_EFFECT               // WON ADD ����ѩ���������Ч 
#include "recv.h"
#endif

#include "defend.h"

#include "char.h"

#include <mysql/mysql.h>

#define BOOL int
#define FALSE 0
#define TRUE 1
MYSQL mysql;
MYSQL_RES *mysql_result;
MYSQL_ROW mysql_row;

typedef struct tagConfig
{
    char    sql_IP[16]; 

		int     sql_Port; 
		
		char    sql_Port1[16];
		
    char    sql_ID[16];
    
    char    sql_PS[16];

    char    sql_DataBase[16];  

    char    sql_Table[16];    

		char    sql_Name[16];   

		char    sql_PassWord[16];   
		
		char    sql_RegTime[16]; 
		
		char    sql_LoginTime[16]; 
		
		char    sql_Path[10];  
		
		char    sql_OnlineName[16]; 

}Config;

Config config;

int AutoReg;

static int readConfig( char *path )
{
    char buf[2048];
    FILE *fp;

    fp = fopen( path , "r" );
    if( fp == NULL ){ return -2; }

    while( fgets( buf , sizeof( buf ) , fp )){
        char command[1024];
        char param[2048];
        chop(buf);
        
        easyGetTokenFromString( buf , 1 , command , sizeof( command ));
        easyGetTokenFromString( buf , 2 , param , sizeof( param ));

        if( strcmp( command , "sql_IP" ) == 0 ){
            strcmp( config.sql_IP , param ) ;
            snprintf( config.sql_IP , sizeof( config.sql_IP) , param );
            log("\n���ݿ��ַ��  %s",config.sql_IP);
        } else if( strcmp( command , "sql_Port" ) == 0 ){
        		config.sql_Port = atoi( param );
            snprintf( config.sql_Port1 , sizeof( config.sql_Port1) , param );
				  	log("\n���ݿ�˿ڣ�  %d",config.sql_Port);
        } else if( strcmp( command , "sql_ID" ) == 0 ){
        		strcmp( config.sql_ID , param ) ;
            snprintf( config.sql_ID , sizeof( config.sql_ID) , param );
						log("\n���ݿ��û���  %s",config.sql_ID);
        } else if( strcmp( command , "sql_PS" ) == 0 ){
        		strcmp( config.sql_PS , param ) ;
            snprintf( config.sql_PS , sizeof( config.sql_PS) , param );
						log("\n���ݿ����룺  %s",config.sql_PS);
        } else if( strcmp( command , "sql_DataBase" ) == 0 ){
        		strcmp( config.sql_DataBase , param ) ;
            snprintf( config.sql_DataBase , sizeof( config.sql_DataBase) , param );
						log("\n��½���ݿ�����%s",config.sql_DataBase);
        } else if( strcmp( command , "sql_Table" ) == 0 ){
        		strcmp( config.sql_Table , param ) ;
            snprintf( config.sql_Table , sizeof( config.sql_Table) , param );
				  	log("\n���ݿ������  %s",config.sql_Table);
        } else if( strcmp( command , "sql_Name" ) == 0 ){
        		strcmp( config.sql_Name , param ) ;
            snprintf( config.sql_Name , sizeof( config.sql_Name) , param );
						log("\n�û����ֶΣ�  %s",config.sql_Name);
        } else if( strcmp( command , "sql_PassWord" ) == 0 ){
        		strcmp( config.sql_PassWord , param ) ;
        		snprintf( config.sql_PassWord , sizeof( config.sql_PassWord) , param );
						log("\n�����ֶΣ�    %s",config.sql_PassWord);
        } else if( strcmp( command , "sql_RegTime" ) == 0 ){
        		strcmp( config.sql_RegTime , param ) ;
            snprintf( config.sql_RegTime , sizeof( config.sql_RegTime) , param );
						log("\nע��ʱ�䣺    %s",config.sql_RegTime);
        } else if( strcmp( command , "sql_LoginTime" ) == 0 ){
        		strcmp( config.sql_LoginTime , param ) ;
            snprintf( config.sql_LoginTime , sizeof( config.sql_LoginTime) , param );
						log("\n��½ʱ�䣺    %s",config.sql_LoginTime);
				} else if( strcmp( command , "sql_Path" ) == 0 ){
        		strcmp( config.sql_Path , param ) ;
            snprintf( config.sql_Path , sizeof( config.sql_Path) , param );
						log("\n·���ֶΣ�    %s",config.sql_Path);
				} else if( strcmp( command , "sql_OnlineName" ) == 0 ){
        		strcmp( config.sql_OnlineName , param ) ;
            snprintf( config.sql_OnlineName , sizeof( config.sql_OnlineName) , param );
						log("\n��������ֶΣ�%s",config.sql_OnlineName);
				} else if( strcmp( command , "AutoReg" ) == 0 ){
        		AutoReg = atoi( param );
        		log("\n�����Զ�ע�᣺%d",AutoReg);
    fclose(fp);
    return 0;
    		}
    }
}

BOOL sasql_init( void )
{
	
if( mysql_init(&mysql) == NULL & readConfig("acserv.cf"))
{
	  log("\n���ݿ��ʼ��ʧ�ܣ�");
	  exit(1);
    return FALSE;
}

  if( !mysql_real_connect( &mysql,
          config.sql_IP,
          config.sql_ID,//�ʺ�
          config.sql_PS,//����
          config.sql_DataBase,//ѡ������Ͽ�
          config.sql_Port,
          NULL,
          0 ) )
	{
		log("\n�������ݿ�ʧ�ܣ�\n");
		return FALSE;
	}
  
  	log("\n���ݿ����ӳɹ���\n");
#ifdef _SQL_BACKGROUND
    if(!sasql_initonline())
      	log("\n���ݿ��ʼ��ʧ�ܣ�\n");
#endif
  	return TRUE;
}

void sasql_close( void )
{
  mysql_close( &mysql );
}


BOOL sasql_query(char *nm, char *pas){
	char sqlstr[256];
	sprintf(sqlstr,"select * from %s where %s=BINARY'%s' and %s=BINARY'%s'",config.sql_Table,config.sql_Name,nm,config.sql_PassWord,pas);
	if(!mysql_query(&mysql,sqlstr)){
	  int num_row=0;
	  mysql_result=mysql_store_result(&mysql);
	  num_row=mysql_num_rows(mysql_result);
	  mysql_free_result(mysql_result);
	  if(num_row==1){
	  	return TRUE;}
	  else{
	  	return FALSE;}}
	else{
		log("\n ���ݿ����ʧ�ܣ�\n");
		return FALSE;}
}


#ifdef _SQL_BACKGROUND
BOOL sasql_update(char *nm, char *path){
	char sqlstr[256];
	sprintf(sqlstr,"update %s set %s='%s' where %s=BINARY'%s'",config.sql_Table,config.sql_Path,path,config.sql_Name,nm);
	if(!mysql_query(&mysql,sqlstr)){
		return TRUE;}
	else{
		log("\n�������ݿ�ʧ�ܣ�\n");
		return FALSE;}
}

BOOL sasql_online(char *nm,char *lnm){
	char sqlstr[256];
	sprintf(sqlstr,"update %s set %s='%s',%s=NOW() where %s=BINARY'%s'",config.sql_Table,config.sql_OnlineName,lnm,config.sql_LoginTime,config.sql_Name,nm);
	if(!mysql_query(&mysql,sqlstr)){
		return TRUE;}
	else{
		log("\n�������ݿ�ʧ�ܣ�\n");
		return FALSE;}
}

BOOL sasql_initonline(){
	char sqlstr[256];
	sprintf(sqlstr,"update %s set %s=NULL",config.sql_Table,config.sql_OnlineName);
	if(!mysql_query(&mysql,sqlstr)){
		return TRUE;}
	else{
		return FALSE;}
}
#endif

#ifdef _SQL_REGISTER
BOOL sasql_register(char *nm, char *pas){
	char sqlstr[256];
	if(AutoReg==1)
	{
		sprintf(sqlstr,"select * from %s where %s=BINARY'%s'",config.sql_Table,config.sql_Name,nm);
		if(!mysql_query(&mysql,sqlstr)){
		  int num_row=0;
		  mysql_result=mysql_store_result(&mysql);
		  num_row=mysql_num_rows(mysql_result);
		  mysql_free_result(mysql_result);
		  if(num_row==0){
				sprintf(sqlstr,"INSERT INTO %s (%s,%s,%s) VALUES ('%s','%s',NOW())",config.sql_Table,config.sql_Name,config.sql_PassWord,config.sql_RegTime,nm,pas);
				if(!mysql_query(&mysql,sqlstr)){
					log("\n���û�ע��ɹ���\n");
					return TRUE;}
				else{
					log("\n���û�ע��ʧ�ܣ�\n");
				return FALSE;}}
		  else{
		  	return FALSE;}}
		else{
			log("\n ���ݿ����ʧ�ܣ�\n");
			return FALSE;}
	}
	else{
		return FALSE;}
}
#endif

#else
/* Stub implementations when _SASQL1 is not defined */

#define BOOL int
#define TRUE 1
#define FALSE 0

BOOL sasql_query(char *nm, char *pas) {
    /* No SQL, always return TRUE (account valid) */
    return TRUE;
}

BOOL sasql_register(char *nm, char *pas) {
    /* No SQL, registration not supported */
    return FALSE;
}

BOOL sasql_init(void) {
    return TRUE;
}

void sasql_close(void) {
}

#endif