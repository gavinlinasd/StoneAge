#ifndef _SAACPROTO_OAC_H
#define _SAACPROTO_OAC_H

#ifdef _OACSTRUCT_TCP
#define OAC_PORT 9099
enum{
	OACTYPE_ERROR=0,
	OACTYPE_OK=1,
	OACTYPE_BIND=2,
	OACTYPE_CONNECT=3,
};

typedef struct _tagOACListArray{
	char addr[64];
	char name[64];
	char fd;
	int use;
}OACListArray;

int OAC_InitTcpIp( int tcptype, char *addr, int tport);
int OAC_ConnectOacList( void);
int OAC_ConnectOacOne( int ti);
int OAC_CheckConnect( void);
int OAC_select( void );
int OAC_ReadMessLine( void);

//DispatchMessage--------------------------------------------------------------

int OAC_ACDispatchMessage( int fd , char *encoded, char *debugfun);

void OAC_ACLogin_recv( int fd, char* data );
void OAC_ACLogin_send( int fd, char* data );
void OAC_ACCharLoad_recv( int fd, char *id, char *charname, int optti, int clifd, int gsfd);
void OAC_ACCharLoad_send( int fd, char *id, char *charname, int optti, int clifd, int gsfd);
void OAC_ACCharSave_recv( int fd, char *id, char *charname, char *opt, int optti, int clifd, int gsfd);
void OAC_ACCharSave_send( int fd, char *id, char *charname, char *opt, int optti, int clifd, int gsfd);

void OAC_Send( int fd , char *msg );
void OAC_InitAclist( void);
void OAC_ReadFileInsertAcList( char *filename);

#endif

#endif
