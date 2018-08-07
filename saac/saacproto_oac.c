#include "version.h"
#include "main.h"
#include "util.h"
#include "char.h"
#include "tcpip_util.h"
#include "saacproto_util.h"
#include "saacproto_oac.h"

#ifdef _OACSTRUCT_TCP

#define BACKLOGNUM 5
int oactcptype = -1;
static int btime = 0;
OACListArray OAC_List[MAXTCPCONNECTION];

int OAC_InitTcpIp( int tcptype, char *addr, int tport)
{
	int tcpdb=0, binport=OAC_PORT;

	//andy_log
	log( "_InitTcpIp( %d)\n", tcptype);
	OAC_InitAclist();

	if( MEMBUF_InitSize( CHARDATASIZE * 4 * MAXTCPCONNECTION , tcpdb ) != OK ){
		return INIT_SOMEERROR;
	}
	if( tport > 0 )
		binport = tport;

	oactcptype = tcptype;
	switch( tcptype){
	case OACTYPE_BIND:
		if( TCPIP_bindSocket( addr , binport, 0) < 0 )
			return INIT_SOMEERROR;
		break;
	case OACTYPE_CONNECT:
		OAC_ReadFileInsertAcList( "oac.lis");
		OAC_ConnectOacList();
//		if( OAC_ConnectOacList() == 0 )
//			return INIT_SOMEERROR;
		break;
	default:

		return INIT_SOMEERROR;
	}
	return OK;
}

int OAC_ConnectOacList( void)
{
	int i=0, cnt=0;
	for( i=0; i<MAXTCPCONNECTION; i++){
		if( OAC_ConnectOacOne( i) < 0 ) continue;
		usleep( 1000);
		cnt++;
	}
/*andy_test=============
	for( i=0; i<MAXTCPCONNECTION; i++){
		if( OAC_List[i].use == 0 ) continue;
		OAC_ACCharLoad_send( i, "ankoo", "123", 0, -1, -1);
		cnt++;
	}
//======================*/
	return cnt;
}


int OAC_ConnectOacOne( int ti)
{
	int tfd;
	if( OAC_List[ti].use == 0 ) return -1;
	if( OAC_List[ti].fd != -1 ) return -1;
	if( (tfd=TCPIP_connect( OAC_List[ti].addr , OAC_PORT )) < 0 ){
		log( "Can't connect OAC:%s", OAC_List[ti].addr);
		return -1;
	}
	log( "connect OAC:%s", OAC_List[ti].addr);
	OAC_List[ti].fd = tfd;
	OAC_ACLogin_send( tfd, "²âÊÔ»ú");
	return tfd;
}

int OAC_CheckConnect( void)
{
	int i, ncon=-1;
//andy_log
log( "_CheckConnect btime:%d\n", btime);
	for( i=0; i<MAXTCPCONNECTION; i++){
		if( OAC_List[i].use == 0 ) continue;
		ncon=OAC_List[i].fd;
		if( ncon == -1 || TCPIP_checkCon( ncon) == 0 ){
			OAC_List[i].fd = -1;
			OAC_ConnectOacOne( i);
			return 1;
		}else{
			//andy_test
			OAC_ACCharLoad_send( i, "ankoo", "123", 0, -1, -1);
		}
	}
	return 0;
}

int OAC_select( void )
{
    int tis[BACKLOGNUM];
    int ret, int detime;
	
	detime = (int)time(NULL);
	TCPIP_selectdata();

	if( oactcptype == OACTYPE_BIND ){
		ret = TCPIP_accept( tis, 1);
		if( ret < 0 ){
			return ret;
		} else if( ret == 1 ){
//andy_log
log( "OAC    SomeOne connect !!\n");
			return tis[0];
		} else {
			return TCPSTRUCT_EBUG ;
		}
	}
	if( detime > btime){
		OAC_CheckConnect();
		btime = detime+180;
	}
	return 1;
}

int OAC_ReadMessLine( void)
{
	int i=0, l;
	char buf[ CHARDATASIZE*4 ];

	for( i=0; i<MAXTCPCONNECTION; i++){
		if( TCPIP_checkCon( i) == 0 ) continue;
		l = MEMBUF_readline_chop( i , buf, sizeof(buf) -1 );
		if( l == TCPSTRUCT_ETOOLONG || l < 0 ){
			//andy_log
			log( "_close( %d)!\n", i);
			TCPIP_close( i);
		}else if( l == 0 ){
			;
		}else{
			char debugfun[256];
//andy_log
//log( "OAC readline:%s len:%d !!\n", buf, l);
			
			buf[l]=0;
			if( OAC_ACDispatchMessage( i , buf, debugfun) < 0 ){
				TCPIP_close(i);
				log( "OAC(%d) Message error:%s\n", i, debugfun);
			}
		}
	}
	return 1;
}

//DispatchMessage--------------------------------------------------------------

int OAC_ACDispatchMessage( int fd , char *encoded, char *debugfun)
{
	unsigned int msgid;
	char funcname[256];

	saacproto_splitString( encoded );
	saacproto_GetMessageInfo(&msgid , funcname ,sizeof(funcname), saacproto.token_list );

	if( strcmp( funcname , "OACLogin" ) == 0 ){
		char* oacname;
		oacname = saacproto_wrapStringAddr( saacproto_stringwrapper[1] , saacproto.workbufsize , saacproto_demkstr_string( saacproto.token_list[2] ));
		OAC_ACLogin_recv( fd, oacname);
		return 0;
	}

	if( strcmp( funcname , "OACProbe" ) == 0 ){
		return 0;
	}

	if( strcmp( funcname , "OACCharLoad" ) == 0 ){
		char *id, *charname;
		int optti, clifd, gsfd;
		id = saacproto_wrapStringAddr( saacproto_stringwrapper[1] , saacproto.workbufsize , saacproto_demkstr_string( saacproto.token_list[2] ));
		charname = saacproto_wrapStringAddr( saacproto_stringwrapper[2] , saacproto.workbufsize , saacproto_demkstr_string( saacproto.token_list[3] ));
		optti = saacproto_demkstr_int( saacproto.token_list[4] );
		clifd = saacproto_demkstr_int( saacproto.token_list[5] );
		gsfd = saacproto_demkstr_int( saacproto.token_list[6] );
		OAC_ACCharLoad_recv( fd, id, charname, optti, clifd, gsfd);
		return 0;
	}

	if( strcmp( funcname , "OACCharSave" ) == 0 ){
		char *id, *charname, *opt;
		int optti, clifd, gsfd;
		id = saacproto_wrapStringAddr( saacproto_stringwrapper[1] , saacproto.workbufsize , saacproto_demkstr_string( saacproto.token_list[2] ));
		charname = saacproto_wrapStringAddr( saacproto_stringwrapper[2] , saacproto.workbufsize , saacproto_demkstr_string( saacproto.token_list[3] ));
		opt = saacproto_wrapStringAddr( saacproto_stringwrapper[3] , saacproto.workbufsize , saacproto_demkstr_string( saacproto.token_list[4] ));
		optti = saacproto_demkstr_int( saacproto.token_list[5] );
		clifd = saacproto_demkstr_int( saacproto.token_list[6] );
		gsfd = saacproto_demkstr_int( saacproto.token_list[7] );
		OAC_ACCharSave_recv( fd, id, charname, opt, optti, clifd, gsfd);
		return 0;
	}

	sprintf( debugfun, "%s", funcname);
	return -1;
}
//---------------------------------------------------------------------------
void OAC_ACLogin_recv( int fd, char* data )
{
	//andy_log
	log( "_ACLogin_recv( %d, %s) \n", fd, data);
	sprintf( OAC_List[fd].name, "%s", data);
	OAC_List[fd].use = 1;
}
void OAC_ACLogin_send( int fd, char* data )
{
	saacproto_CreateHeader( saacproto.work , "OACLogin" );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_string( data ) ,saacproto.workbufsize );
	OAC_Send( fd , saacproto.work );
}
//---------------------------------------------------------------------------
void OAC_ACCharLoad_recv( int fd, char *id, char *charname, int optti, int clifd, int gsfd)//ÒªÇó
{
	int charindex;
	char loadbuf[CHARDATASIZE];
	//andy_log
	log( "_ACCharLoad_recv( %d, %s, %s, %d)\n", fd, id, charname, optti);

#if 1
	charindex = getCharIndexByName( id , charname );
	if( charindex < 0 ){
		OAC_ACCharSave_send( fd, id, charname, FAILED, optti, clifd, gsfd);
	}
	if( loadCharOne( id , charindex ,  loadbuf , sizeof( loadbuf )-1) <0){
		OAC_ACCharSave_send( fd, id, charname, FAILED, optti, clifd, gsfd);
	}else{
		OAC_ACCharSave_send( fd, id, charname, loadbuf, optti, clifd, gsfd);
	}
#else
	if( loadCharOne( id , optti ,  loadbuf ,sizeof( loadbuf ) -1) < 0 ){
		OAC_ACCharSave_send( fd, id, charname, FAILED, optti, clifd, gsfd);
	}else{
		OAC_ACCharSave_send( fd, id, charname, loadbuf, optti, clifd, gsfd);
	}
#endif

}

void OAC_ACCharLoad_send( int fd, char *id, char *charname, int optti, int clifd, int gsfd)
{
	saacproto_CreateHeader( saacproto.work , "OACCharLoad" );
	saacproto_strcatsafe( saacproto.work, saacproto_mkstr_string( id), saacproto.workbufsize );
	saacproto_strcatsafe( saacproto.work, saacproto_mkstr_string( charname), saacproto.workbufsize );
	saacproto_strcatsafe(saacproto.work , saacproto_mkstr_int(optti) ,saacproto.workbufsize );
	saacproto_strcatsafe(saacproto.work , saacproto_mkstr_int(clifd) ,saacproto.workbufsize );

	OAC_Send( fd , saacproto.work );
}
//---------------------------------------------------------------------------
void OAC_ACCharSave_recv( int fd, char *id, char *charname, char *opt, int optti, int clifd, int gsfd)
{
	//andy_log
	log( "_ACCharSave_recv( %d, %s, %s, %d):%d\n", fd, id, charname, optti, strlen( opt));

//	log( "_ACCharSave_recv( %d, %s, %s, %d)\nopt:%s\n", fd, id, charname, optti, opt);
}
void OAC_ACCharSave_send( int fd, char *id, char *charname, char *opt, int optti, int clifd, int gsfd)
{
	saacproto_CreateHeader( saacproto.work , "OACCharSave" );
	saacproto_strcatsafe( saacproto.work, saacproto_mkstr_string( id), saacproto.workbufsize );
	saacproto_strcatsafe( saacproto.work, saacproto_mkstr_string( charname), saacproto.workbufsize );
	saacproto_strcatsafe( saacproto.work, saacproto_mkstr_string( opt), saacproto.workbufsize );
	saacproto_strcatsafe(saacproto.work , saacproto_mkstr_int(optti) ,saacproto.workbufsize );
	saacproto_strcatsafe(saacproto.work , saacproto_mkstr_int(clifd) ,saacproto.workbufsize );

	OAC_Send( fd , saacproto.work );
}
//---------------------------------------------------------------------------
void OAC_Send( int fd , char *msg )
{
    char *encoded;
    encoded = msg;
	{
		unsigned int l = strlen( encoded );
		if( l < saacproto.workbufsize *3){
			encoded[l] = '\n';
			encoded[l+1] = 0;
			l++;
		}else{
			//andy_log
			log( "OAC l:%d < workbufsize:%d err : \n (%s)\n", l, saacproto.workbufsize, msg );
		}
		TCPIP_send( fd, encoded, l);
	}
}

void OAC_InitAclist( void)
{
	int i;
	for( i=0; i<MAXTCPCONNECTION; i++){
		OAC_List[i].use = 0;
		OAC_List[i].fd = -1;
	}
}

void OAC_ReadFileInsertAcList( char *filename)
{
	FILE *fp=NULL;
	int cnt=0;
	char buf[512], buf1[256];

	if( (fp=fopen( filename, "r")) == NULL ) return;

	while( fgets( buf, sizeof(buf)-1, fp) ){
		if( buf[0] == '#' || buf[0] == '\n' ||
			buf[0] == 0 ) continue;
		remove_r( buf );
		easyGetTokenFromBuf( buf, ':', 1, buf1, sizeof( buf1));
		sprintf( OAC_List[cnt].name, "%s", buf1);

		easyGetTokenFromBuf( buf, ':', 2, buf1, sizeof( buf1));
		sprintf( OAC_List[cnt].addr, "%s", buf1);
		OAC_List[cnt].use = 1;
		cnt++;
		if( cnt >= MAXTCPCONNECTION ) break;
	}
	fclose( fp);
	{//andy_log
		int i;
		for( i=0; i<MAXTCPCONNECTION; i++){
			if( OAC_List[i].use == 0 ) continue;
			log( "OAC_List[%d] %s %s .\n", i, OAC_List[i].name, OAC_List[i].addr)
		}
	}
}

#endif
