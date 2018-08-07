#ifndef _SAACPROTOWORK_H_
#define _SAACPROTOWORK_H_
#include "version.h"
#include "main.h"
#ifdef _FIX_WORKS

struct connection *cWork;
//struct membuf *wb;
#define MAXWORKCONNECTION MAXWK	//30
#define WK_R	1
#define WK_W	2

typedef struct tagMEMBuffers{
	char buff[256];
	int use;
	int WR;
}MEMBuffers;

void MEMBuffer_Init();
int MEMBuffer_AddWkReadBuffer( int ti, char *data, int flg);
int MEMBuffer_getWkLineReadBuffer( int ti, char *data, int sizes, int flg);

void MEMBuffer_clean( int ti);
int MEMBuffer_Find( int ti);
int SetWorkConnectionInit( int mem_use);

int tcpstruct_work_accept( int *tis , int ticount );

int cWork_write( int ti , char *buf , int len );
int Work_close( int ti );

#endif


#endif

