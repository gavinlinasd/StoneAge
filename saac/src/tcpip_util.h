#ifndef _TCPIPAC_H
#define _TCPIPAC_H
#include "version.h"

#ifdef _TCPIP_UTIL
#define MAXTCPCONNECTION 15
int MEMBUF_InitSize( int mem_use, int db );
int MEMBUF_unregMemBuf(  int ti );
int MEMBUF_findregBlankMemBuf( void  );
int MEMBUF_findregBlankCon( void );
int MEMBUF_getFreeMem( void );
int MEMBUF_appReadBuffer(  int ti, char *data, int len);
int MEMBUF_appWriteBuffer( int ti, char *data, int len );
int MEMBUF_appMemBufList( int top , char *data , int len );
int MEMBUF_consumeMemBufList( int top, char *out, int len, int
							 consumeflag, int copyflag );
int MEMBUF_getLineReadBuffer( int ti , char *buf, int len );
int MEMBUF_readline( int ti , char *buf , int len , int kend , int kend_r );
int MEMBUF_readline_chop( int ti , char *buf, int len );
int MEMBUF_writeline( int ti , char *buf , int len );

//-----------------------------------------------------------------------------

int TCPIP_bindSocket( char *addr , int p , int timeout_ms);
int TCPIP_connect( char *addr , int port );
int TCPIP_close( int ti );
int TCPIP_selectdata( void);
int TCPIP_accept( int *tis , int ticount );
int TCPIP_send( int ti , char *buf , int len );
int TCPIP_checkCon( int ti);

#endif
#endif
