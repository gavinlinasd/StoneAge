#ifndef _LSERVER_H
#define _LSERVER_H

#include "version.h"

#ifdef _LOCK_SERVER
#define _DEFLSCONNECT
#ifndef BOOL
#define BOOL int
#define TRUE 1
#define FALSE 0
#endif


enum {
	LS_PORT = 0,
	LS_LASTTIME,
	LS_FD,
	LS_USE,
	LS_TYPE,
	LS_TI,
	LS_LASTERROR,
	LS_MAXDATA,
};

enum {
	LSTYPE_NORMAL=0,
	LSTYPE_STANDBY,
	LSTYPE_BEDIE,

	LS_OK = 1000,
	LSMESS_NOCONNECT,
	LSMESS_BELIFE,
	LSMESS_SOCKERROR,
	LSMESS_HOSTERROR,
	LSMESS_READERROR,
	LSMESS_WRITEERROR,
	LSMESS_LSMEMERROR,
};

typedef struct MemMessage
{
	char buf[4096];
	int type;
	struct MemMessage *next;
}LSMem;

#define MEMREAD 1
#define MEMWRITE 2
typedef struct _LockServer
{
	char address[256];
	int data[LS_MAXDATA];
	int errs;
	int m_ri;
	int m_wi;
}LServer;
extern LServer Ls;


BOOL LServerInit();
BOOL connectToLServer( char *laddress, int lport);
BOOL DisConnectLServer();
int tcpip_connect( char *addr , int port);
int LS_getData( int stype);
BOOL Add_LS_WRMemBuffer( char *buf, int strlens, int flg);
LSMem *MEMBUF_getNew();
void DEL_LS_MemBufferList( LSMem *FlgMem );
int MMGet_LS_WRMemBufList(  char *buf, int len, int flg);
BOOL TcpIp_LS_accept();
int MMGet_LsReadBuflist( char *buf, int len);
BOOL CHECK_LS_BEUSE();
void CHECK_LSTYPE();
int get_LSFD();
void Show_LS_MEMBufferList();
void DEL_LS_MemBufferHead();

typedef struct _SAMessCommand
{
	char buf[256];
	int len;
	struct _SAMessCommand *next;
}SAMessC;
BOOL SAMESS_setChar( char *buf);
BOOL SAMESS_getChar( char *buf, int *len);

SAMessC *MESS_getNew();

int lockproto_SaacDispatchMessage( char *encoded );

void LServer_SALogin_send();
void LServer_SACheckLock1_send( int ti, char *Pcdkey, int Pid, int flg);
void LServer_SACheckLock1_recv( char *si, char *Pcdkey, char *Pid, char *flg, char *PASS);
void LServer_SACheckLock2_send( int ti, char *Pcdkey, int Pid, char *charname, int flg, char *pro);
void LServer_SACheckLock2_recv( char *si, char *Pcdkey, char *Pid, char *name,
							   char *flg, char *pro);
void LServer_SALock_recv( char *si, char *Pcdkey, char *Pid, char *name, char *flg);
void LServer_SALock_send( int ti, char *Pcdkey, int Pid, char *Pname, int flg);
void LServer_SAUNLock_recv( char *si, char *Pcdkey, char *Pid, char *flg);
//si[256], PCDKEY[256], Pid[256], FLG[256];
void LServer_SAUNLock_send( int ti, char *Pcdkey, int Pid, int flg);
//check
void LServer_SAUCheck_recv( char *Pcdkey);
void saacproto_SAUCheck_send( char *Pcdkey, int flg);

void DefTimeC( char *buf, int flg);
#ifdef _TEST_LOCKSERVER
void TEST_TOLOCKSERVER();
#endif
#endif
#endif


