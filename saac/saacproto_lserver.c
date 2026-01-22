#include "saacproto_lserver.h"
#include "saacproto_serv.h"
#include "main.h"
#include "char.h"
#include "util.h"
#include "version.h"


#ifdef _LOCK_SERVER

LServer Ls;
LSMem *LSbufHead;
SAMessC *SAMess;

struct timeval select_timeout;
void REreadConfig( char *path );
BOOL LSbuffInit()
{
	//LSbufHead->next = NULL;
	//LSbufHead = NULL;
	return TRUE;
}

BOOL LServerInit()
{
	int i;
	if( Ls.data[LS_FD] >= 0 )
		close( Ls.data[LS_FD]);
	for( i=0;i<LS_MAXDATA;i++)	{
		Ls.data[i] = 0;
	}
	Ls.data[LS_TYPE] = LSTYPE_NORMAL;
	Ls.data[LS_FD] = -1;
	memset( Ls.address, 0, sizeof( Ls.address));
	Ls.errs = 0;
	return TRUE;
}

BOOL connectToLServer( char *laddress, int lport)
{
    int ti;
	if( Ls.data[LS_USE] )	{
		return FALSE;
	}
	if( (ti = tcpip_connect( laddress, lport)) == LS_OK)	{
		Ls.data[LS_TI] = ti;
		Ls.data[LS_USE] = 1;
		Ls.data[LS_LASTTIME] = 0;
		Ls.data[LS_TYPE] = LSTYPE_STANDBY;
		Ls.data[LS_PORT] = lport;
		Ls.errs = 0;
		snprintf( Ls.address, sizeof( Ls.address),"%s", laddress);
		Ls.data[LS_LASTERROR] = -1;
		log("LS[use:%d, FD:%d, TI:%d, address:%s, port:%d]...\n", Ls.data[LS_USE], Ls.data[LS_FD],
			Ls.data[LS_TI], Ls.address, Ls.data[LS_PORT]);
	} else {
		Ls.data[LS_LASTERROR] = ti;
		return FALSE;
	}
	return TRUE;
}

BOOL DisConnectLServer()
{
	if( Ls.data[LS_USE])	{
		Ls.data[LS_LASTERROR] = LSMESS_NOCONNECT;
		Ls.data[LS_USE] = 0;
		Ls.errs = 0;
		return FALSE;
	}

    close( Ls.data[LS_FD]);
	Ls.data[LS_FD] = -1;
	LServerInit();

	while( LSbufHead != NULL )	{
		DEL_LS_MemBufferHead();
	}
    return TRUE;
}

int tcpip_connect( char *addr , int port)
{
    int s, r;
    struct sockaddr_in svaddr;
    struct hostent *he;
    
    s = socket( AF_INET, SOCK_STREAM , 0 );
    if(s<0) return LSMESS_SOCKERROR;
    bzero( &svaddr , sizeof( svaddr ));
    svaddr.sin_family = AF_INET;
    svaddr.sin_port = htons( port );

    if( inet_aton( addr, &svaddr.sin_addr ) == 0 ){
        he = gethostbyname( addr );
        if( he == NULL ){
            return LSMESS_HOSTERROR;
        }
        memcpy( & svaddr.sin_addr.s_addr , he->h_addr, sizeof( struct in_addr));
    }
    r = connect( s , ( struct sockaddr*)&svaddr,sizeof(svaddr));
    if( r < 0 ) return LSMESS_NOCONNECT;
    set_nodelay( s );
	Ls.data[LS_FD] = s;
    return LS_OK;
}
int LS_getData( int stype)
{
	return Ls.data[stype];
}

BOOL TcpIp_LS_accept()
{
    int sret;
    struct timeval t;
	char tmpbuf[4096];
	char send_buf[4096];
	int lsfd = Ls.data[LS_FD];
	LSMem *FlgMem;
    fd_set rfds, wfds , efds;

	FD_ZERO( &rfds );
	FD_ZERO( &wfds );
	FD_ZERO( &efds );
	
    select_timeout.tv_sec = 0;
    select_timeout.tv_usec = 0;

	if( Ls.data[LS_USE] && (lsfd >= 0 ) && !Ls.errs )	{
		FD_SET( lsfd, &rfds );
		FD_SET( lsfd, &wfds );
		FD_SET( lsfd, &efds );
	}else	{
		log(" LS error t1!!\n");
		return FALSE;
	}

    t = select_timeout;
    sret = select( lsfd+1, &rfds , (fd_set*)NULL, &efds , &t);
	if( sret > 0 ) {
		memset( tmpbuf, 0, sizeof( tmpbuf));
		if( ( lsfd >= 0 ) && FD_ISSET( lsfd , &rfds ) ){
			int rr;
			rr = read( lsfd, tmpbuf , sizeof( tmpbuf));
			if( rr <= 0 ){
				Ls.errs = 1;
				Ls.data[LS_LASTERROR] = LSMESS_READERROR;
			} else {
				Add_LS_WRMemBuffer( tmpbuf , rr, MEMREAD); //增加讯息
			}
		}
	}

    t = select_timeout;    
    sret = select( lsfd+1, (fd_set*)NULL, &wfds, &efds , &t);
	if( sret > 0 ) {
		if( ( lsfd >= 0 ) && FD_ISSET( lsfd , &wfds )){
			int l , rr, writesize;
			FlgMem = NULL;
			memset( send_buf, 0, sizeof( send_buf));
			l = MMGet_LS_WRMemBufList( send_buf, sizeof( send_buf), MEMWRITE);
			if( l > 0 ){
				writesize = strlen( send_buf)+1;
				rr = write( lsfd , send_buf , writesize);
				if( rr < 0 ){
					Ls.errs = 1;
				}
			}
		}
	}

    return TRUE;
}

BOOL Add_LS_WRMemBuffer( char *buf, int strlens, int flg)
{
	int i=0;
	LSMem *workLS, *FindLs;
	FindLs = LSbufHead;

	if( Ls.data[LS_LASTTIME] >= 50 )	{
		log( "LS Mess busy:%d\n", Ls.data[LS_LASTTIME]);
		return FALSE;
	}
	while ( 1 )	{
		if( FindLs == NULL || FindLs->next == NULL )
			break;
		FindLs = FindLs->next;
		i++;
	}

	Ls.data[LS_LASTTIME] = i;
	workLS = MEMBUF_getNew();
	if( workLS == NULL )
		return FALSE;
	memcpy( workLS->buf, buf, sizeof( workLS->buf));
	workLS->type = flg;
	workLS->next = NULL;

	if( LSbufHead == NULL )
		LSbufHead = workLS;
	else	{
		FindLs->next = workLS;
	}

	return TRUE;
}

LSMem *MEMBUF_getNew()
{
	LSMem *MBUF=NULL;
	MBUF = (LSMem *) calloc( 1, sizeof( struct MemMessage) );
	if( MBUF == NULL )
		return NULL;
	MBUF->type = 0;
	return MBUF;
}

int MMGet_LsReadBuflist( char *buf, int len)
{
	int l;
	char buf1[256];
	LSMem *FlgMem;
	FlgMem = NULL;
	l = MMGet_LS_WRMemBufList( buf1, len, MEMREAD);
	if( l > 0 )	{
		memcpy( buf, &buf1, 256);
		return strlen( buf1);
	}
	return 0;
}

int MMGet_LS_WRMemBufList(  char *buf, int len, int flg)
{
	LSMem *workLS;
	LSMem *POINT1=NULL;
	workLS = LSbufHead;
	POINT1 = NULL;
	while( workLS != NULL ){
		if( workLS->type == flg )	{
			sprintf( buf, "%s", workLS->buf);
			if( POINT1 == NULL )	{
				LSbufHead = workLS->next;
			}else	{
				POINT1->next = workLS->next;
			}
			free ( workLS);
			return 1;
		}
		POINT1 = workLS;
		workLS = workLS->next;
	}
	return 0;
}

void DEL_LS_MemBufferHead()
{
	LSMem *workLS;
	if( LSbufHead == NULL )
		return;
	if( LSbufHead->next == NULL )
		return;
	workLS = LSbufHead;
	LSbufHead = LSbufHead->next;
	free( workLS);
}

void DEL_LS_MemBufferList( LSMem *FlgMem )
{
	LSMem *workLS=NULL;
	LSMem *POINT1=NULL;

	workLS = LSbufHead;
	POINT1 = NULL;
	while( workLS != NULL)	{
		if( workLS == FlgMem )	{
			if( POINT1 == NULL )	{
				LSbufHead = FlgMem->next;
			}else	{
				POINT1->next = FlgMem->next;
			}
			free( FlgMem);
			return;
		}else	{
			POINT1 = workLS;
			workLS = workLS->next;
		}
	}
}

BOOL CHECK_LS_BEUSE()
{
	if( Ls.data[LS_USE] == 1)
		return TRUE;
	else
		return FALSE;
}

void CHECK_LSTYPE()
{
	static time_t standbyTime;

	if( Ls.data[LS_USE] ){
		if( Ls.errs )	{
			if( DisConnectLServer() == FALSE )	{
				log( "LS Close..\n");
			}else	{
				log( "LS Close error ..!!\n");
			}
		}
		standbyTime = time(NULL);
	}else	{
		if( (standbyTime + 60 ) > time(NULL) )
			return;

		REreadConfig( "saac.cf");
		LServerInit();
		usleep( 200*10000);
		standbyTime = time(NULL);
		if( connectToLServer( LSADDRESS, LSPORT) == FALSE )	{
			log("%s", "LOGIN LServer FAIL!!\n");
			return;
		}else	{
			usleep( 100*10000);
			LServer_SALogin_send();
		}
	}
	//if( Ls.data[LS_LASTTIME] >= 10 )

}

int get_LSFD()
{
	if( Ls.data[LS_USE] )	{
		return Ls.data[LS_FD];
	}
	return -1;
}

void Show_LS_MEMBufferList()
{
	LSMem *workLS=NULL;

	workLS = LSbufHead;
	while( workLS != NULL )	{
		log( "[ %s] ", workLS->buf);
		workLS = workLS->next;
	}

}

int lockproto_SaacDispatchMessage( char *encoded )
{
	int lens;
	char funcname[1024];
	if( SAMESS_setChar( encoded) == FALSE )
		return -1;
	
	memset( funcname, 0, sizeof( funcname));
	if( SAMESS_getChar( funcname, &lens) == FALSE )	{
		log("funcname:NULL\n");
		return 0;
	}

	if( !strcmp( funcname , "TEST" )){
		char buf1[256],buf2[256],buf3[256];
		if( SAMESS_getChar( buf1, &lens) == FALSE ) return 0;
		if( SAMESS_getChar( buf2, &lens) == FALSE ) return 0;
		if( SAMESS_getChar( buf3, &lens) == FALSE ) return 0;
		return 0;
	}

	if( !strcmp( funcname , "SACheckLock1" )){
		char si[256], Pcdkey[256], Pid[256], flg[256], PASS[256];
		if( SAMESS_getChar( si, &lens) == FALSE ) return 0;
		if( SAMESS_getChar( Pcdkey, &lens) == FALSE ) return 0;
		if( SAMESS_getChar( Pid, &lens) == FALSE ) return 0;
		if( SAMESS_getChar( flg, &lens) == FALSE ) return 0;
		if( SAMESS_getChar( PASS, &lens) == FALSE ) return 0;
		LServer_SACheckLock1_recv( si, Pcdkey, Pid, flg, PASS);
		return 0;
	}

	if( !strcmp( funcname , "SACheckLock2" )){
		char si[256], Pcdkey[256], Pid[256], name[256], flg[256], PASS[256];
		if( SAMESS_getChar( si, &lens) == FALSE ) return 0;
		if( SAMESS_getChar( Pcdkey, &lens) == FALSE ) return 0;
		if( SAMESS_getChar( Pid, &lens) == FALSE ) return 0;
		if( SAMESS_getChar( name, &lens) == FALSE ) return 0;
		if( SAMESS_getChar( flg, &lens) == FALSE ) return 0;
		if( SAMESS_getChar( PASS, &lens) == FALSE ) return 0;
		LServer_SACheckLock2_recv( si, Pcdkey, Pid, name, flg, PASS);
		return 0;
	}

	if( !strcmp( funcname , "SALock" )){
		char si[256], Pcdkey[256], Pid[256], name[256], flg[256];

		if( SAMESS_getChar( si, &lens) == FALSE ) return 0;
		if( SAMESS_getChar( Pcdkey, &lens) == FALSE ) return 0;
		if( SAMESS_getChar( Pid, &lens) == FALSE ) return 0;
		if( SAMESS_getChar( name, &lens) == FALSE ) return 0;
		if( SAMESS_getChar( flg, &lens) == FALSE ) return 0;
		LServer_SALock_recv( si, Pcdkey, Pid, name, flg);
		return 0;
	}

	if( !strcmp( funcname , "SAUNLock" )){
		char si[256], Pcdkey[256], Pid[256],  flg[256];

		if( SAMESS_getChar( si, &lens) == FALSE ) return 0;
		if( SAMESS_getChar( Pcdkey, &lens) == FALSE ) return 0;
		if( SAMESS_getChar( Pid, &lens) == FALSE ) return 0;
		if( SAMESS_getChar( flg, &lens) == FALSE ) return 0;
		LServer_SAUNLock_recv( si, Pcdkey, Pid, flg);
		return 0;
	}

	if( !strcmp( funcname , "SAUCheck" )){	//check
		char PCDKEY[256];
		if( SAMESS_getChar( PCDKEY, &lens) == FALSE ) return 0;
		LServer_SAUCheck_recv( PCDKEY);
		return 0;
	}

	return -1;
}

BOOL SAMESS_setChar( char *buf)
{
	char Mbuf[4096];
	int i=0,j=0;
	int len=0;
	SAMessC *Works;
	SAMessC *point;
	point = NULL;

	memset( Mbuf, 0, sizeof( Mbuf));
	sprintf( Mbuf, "%s", buf);
	SAMess = NULL;
	while( Mbuf[i] != '\0' )	{
		if( Mbuf[i] == '\n' )
			Mbuf[i] = '\0';
		i++;
	}
	i=0;
	len = 0;
	while( Mbuf[i] != '\0' && Mbuf[j] != '\0' )	{
		j=i;
		len = 0;
		while( Mbuf[j] != ' ' && Mbuf[j] != '\0')	{
			len++;
			j++;
		}
		Works = MESS_getNew();
		if( Works == NULL )
			return FALSE;
		memcpy( Works->buf, &Mbuf[i], len);
		Works->len = len;
		if( SAMess == NULL )	{
			SAMess = Works;
			point = Works;
		}else	{
			point->next = Works;
			point = point->next;
		}
		i = (j+1);
	}
	return TRUE;
}


BOOL SAMESS_getChar( char *buf, int *len)
{
	SAMessC *Works;
	Works = SAMess;
	*len = 0;
	if( Works == NULL )
		return FALSE;
	sprintf( buf, "%s", Works->buf);
	*len = Works->len;
	SAMess = Works->next;
	free( Works);
	return TRUE;
}

SAMessC *MESS_getNew()
{
	SAMessC *MBUF=NULL;
	MBUF = (SAMessC *) calloc( 1, sizeof( struct _SAMessCommand) );
	if( MBUF == NULL )
		return NULL;
	memset( MBUF->buf, 0, sizeof( MBUF->buf));
	MBUF->len = 0;
	MBUF->next = NULL;
	return MBUF;
}


void LServer_SALogin_send()
{
	//Ls.data[LS_FD]
	char buf[4096];
	sprintf( buf, "SALogin %s %d %s\n", SaName, 0, "LOGIN");
	if( Add_LS_WRMemBuffer( buf, sizeof( buf), MEMWRITE) == FALSE )	{
		log( "LS WRITE ERROR!!");
	}
}

void LServer_SACheckLock1_recv( char *si, char *Pcdkey, char *Pid, char *flg, char *PASS)
{

#ifndef _TEST_LOCKSERVER
	int Pfd, Flg, seri;
	char listbuf[CHARDATASIZE];
	Pfd = atoi( Pid);
	Flg = atoi( flg);
	seri = atoi( si);


	if( !strcmp( flg, "LOCKED") )	{
        saacproto_ACCharList_send( seri , FAILED , "locked" , Pfd );
		total_ng_charlist++;
	}else	{
		loadCharNameAndOption( Pcdkey , listbuf,sizeof(listbuf));
		saacproto_ACCharList_send( seri ,  SUCCESSFUL , listbuf , Pfd);
		total_ok_charlist++;
	}
#endif
}

void LServer_SACheckLock1_send( int ti, char *Pcdkey, int Pid, int flg)
{
	char buf[4096];
	sprintf( buf, "SACheckLock1 %d %s %d %d %s\n", ti, Pcdkey, Pid, flg, "CHECK");
	if( Add_LS_WRMemBuffer( buf, sizeof( buf), MEMWRITE) == FALSE )	{
		log( "LS WRITE ERROR!!");
	}
}

void LServer_SACheckLock2_send( int ti, char *Pcdkey, int Pid, char *charname, int flg, char *pro)
{
	char buf[4096];
	sprintf( buf, "SACheckLock2 %d %s %d %s %d %s\n", ti, Pcdkey, Pid, charname, flg, pro);
	if( Add_LS_WRMemBuffer( buf, sizeof( buf), MEMWRITE) == FALSE )	{
		log( "LS WRITE ERROR!!");
	}
}

void LServer_SACheckLock2_recv( char *si, char *Pcdkey, char *Pid, char *name, char *flg, char *pro)
{
#ifndef _TEST_LOCKSERVER
	int Pfd, Flg, seri;
	int charindex=-1;
	char loadbuf[CHARDATASIZE];
    char infobuf[CHARDATASIZE];
	Pfd = atoi( Pid);
	Flg = atoi( flg);
	seri = atoi( si);

#define IS_2BYTEWORD( _a_ ) ( (char)(0x80) <= (_a_) && (_a_) <= (char)(0xFF) )


	if( !strcmp( flg, "LOCKED") )	{
        saacproto_ACCharLoad_send( seri , FAILED , "locked" , Pfd, charindex );
	}else	{
		int lock = 1;
		charindex = getCharIndexByName( Pcdkey , name );
		if( charindex < 0 ){
			saacproto_ACCharLoad_send( seri , FAILED , "char nonexistent" , Pfd , charindex );
			return;
		}
		log( "loadCharOne: id:[%s] char:[%s]\n", Pcdkey, name );
		if( loadCharOne( Pcdkey , charindex ,  loadbuf ,sizeof( loadbuf ))<0){
			saacproto_ACCharLoad_send( seri , FAILED ,
									 "cannot load ( disk i/o error?)", Pfd, charindex );
			return;
		}
		{//ttom
		   char *c_ptr;
		   c_ptr=loadbuf;
		   while(*c_ptr!='\0'){
			  if( IS_2BYTEWORD(*c_ptr) ){
				  if(*(c_ptr+1)==' '){
					 *(c_ptr+1)=0x41;
				  }
				  c_ptr++;
				  if(*c_ptr=='\0') break;
			  }
			 c_ptr++;     
		   }
		}//ttom

		if( lock ){
			char result[100];
			char retdata[100];
#ifdef _LOCK_ADD_NAME
			if( lockUser( getGSName(seri) , Pcdkey , name , passwd , 1 , result , sizeof( result ) ,
						   retdata , sizeof( retdata ) , pro , "0")<0){
#else
			if( lockUser( getGSName(seri) , Pcdkey , passwd , 1 , result , sizeof( result ) ,
						   retdata , sizeof( retdata ) , pro , "0")<0){
#endif
				saacproto_ACCharLoad_send( seri , FAILED , "lock FAIL" , Pfd, charindex );
				log(" SAAC lock %s|%s FAIL!!\n", getGSName(seri), Pcdkey);
				return;
			}
		}
		//LS LOCK
		if( CHECK_LS_BEUSE() == TRUE )	{
			LServer_SALock_send( seri, Pcdkey, Pfd, name, 1);
		}

		memset( infobuf , 0 , sizeof( infobuf ));
		getCharInfoFromString( loadbuf , infobuf );
		makeStringFromEscaped( infobuf );
		saacproto_ACCharLoad_send( seri ,  SUCCESSFUL , infobuf , Pfd, charindex );

	}
#endif
}

void LServer_SALock_recv( char *si, char *Pcdkey, char *Pid, char *name, char *flg)
{
	int Pfd,  seri;
	Pfd = atoi( Pid);
	seri = atoi( si);

	if( !strcmp( flg, "FAIL") )	{
		log( "LS Lock %s|%s FAIL!!\n", getGSName(seri), Pcdkey);
	}
}

void LServer_SALock_send( int ti, char *Pcdkey, int Pid, char *Pname, int flg)
{
	//si[256], SANAME[256], PCDKEY[256], PNAME[256], FLG[256];
	char buf[4096];

	sprintf( buf, "SALock %d %s %s %d %s %d\n", ti, SaName, Pcdkey, Pid, Pname, flg);
	if( Add_LS_WRMemBuffer( buf, sizeof( buf), MEMWRITE) == FALSE )	{
		log( "LS WRITE ERROR!!");
	}
}

void LServer_SAUNLock_recv( char *si, char *Pcdkey, char *Pid, char *flg)
{
	int Pfd, seri;
	Pfd = atoi( Pid);
	seri = atoi( si);

	if( !strcmp( flg, "FAIL") )	{
		log( "LS UNLock %s|%s FAIL!!\n", getGSName(seri), Pcdkey);
	}
}

void LServer_SAUNLock_send( int ti, char *Pcdkey, int Pid, int flg)
{
	char buf[4096];

	sprintf( buf, "SAUNLock %d %s %s %d %d\n", ti, SaName, Pcdkey, Pid, flg);
	if( Add_LS_WRMemBuffer( buf, sizeof( buf), MEMWRITE) == FALSE )	{
		log( "LS WRITE ERROR!!\n");
	}
}

void LServer_SAUCheck_recv( char *Pcdkey)
{
    if( isLocked( Pcdkey ) ){
        saacproto_SAUCheck_send( Pcdkey, 1);
		checkGSUCheck( Pcdkey );
        return;
    }else	{
		saacproto_SAUCheck_send( Pcdkey, 0);
		return;
	}
}
void saacproto_SAUCheck_send( char *Pcdkey, int flg)
{
	char buf[4096];

	sprintf( buf, "SAUCheck %s %s %d\n", SaName, Pcdkey, flg);
	if( Add_LS_WRMemBuffer( buf, sizeof( buf), MEMWRITE) == FALSE )	{
		log( "LS WRITE ERROR!!");
	}
}

void DefTimeC( char *buf, int flg)// 1 比较 2 秀时间 0 替换
{
	static long int dTimes = 0;
	time_t defLsTime;

	defLsTime = time(NULL);
	if( flg == 1 )	{
		if( defLsTime !=  dTimes )	{
			log( "Time[%s]:%ld|%ld|%ld..\n", buf, defLsTime, dTimes, (defLsTime-dTimes));
			dTimes = defLsTime;
		}
	}else if( flg == 2 )	{
		log( "Time[%s]:%ld..\n", buf, defLsTime);
	}else	{
		dTimes = defLsTime;
	}
}

void REreadConfig( char *path )
{
    char buf[2048];
    FILE *fp;

    fp = fopen( path , "r" );
    if( fp == NULL )
		return;

    while( fgets( buf , sizeof( buf ) , fp )){
        char command[1024];
        char param[2048];
        chop(buf);
        
        easyGetTokenFromString( buf , 1 , command , sizeof( command ));
        easyGetTokenFromString( buf , 2 , param , sizeof( param ));

		if( strcmp( command, "SANAME" ) == 0 ){
            snprintf( SaName, sizeof( SaName ), param );
        } else if( strcmp( command, "LSADDRESS" ) == 0 ){
            snprintf( LSADDRESS, sizeof( LSADDRESS ), param );
        } else if( strcmp( command, "LSPORT" ) == 0 ){
			LSPORT = atoi( param);
        }
    }

    fclose(fp);

}

#ifdef _TEST_LOCKSERVER
void TEST_TOLOCKSERVER()
{
	static int po=0;
	static time_t defLsTime;
	//char charname[256]={"ankoo"};
	//char Pcdkey[256];
	int mesgid = -1;
	int seri=-1;
	
	if( !Ls.data[LS_USE] )
		return;
	if( defLsTime == time( NULL) )
		return;
	defLsTime = time( NULL);

	//sprintf( Pcdkey,"%s", charname);
	switch( po)	{
		case 0:
			LServer_SACheckLock1_send( seri, TESTNAME, mesgid, 1);
			LServer_SACheckLock2_send( seri, TESTNAME, mesgid, TESTNAME, 1, "-1");
			break;
		case 1:
			LServer_SALock_send( seri, TESTNAME, mesgid, TESTNAME, 1);
			break;
		case 2:
			LServer_SAUNLock_send( seri, TESTNAME, mesgid, 1);
			break;
	}
	po++;
	po = po%3; 
}
#endif

#endif
