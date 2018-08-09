#include "version.h"
#include "main.h"
#include "tcpip_util.h"

#ifdef _TCPIP_UTIL
struct connection *tcpcon;
struct membuf *mems=NULL;
static int me_find=0;
static int tpsockfd1=-1;
int mesize;
int meuse;
char tmpbuf[65536];

#define BACKLOGNUM 5
struct timeval select_timeout;
struct sockaddr_in localaddr;

int MEMBUF_InitSize( int mem_use, int db )
{
	mesize = mem_use / sizeof( struct membuf );
    meuse = 0;
    me_find = 0;
	mems = ( struct membuf * ) calloc( 1, mesize * sizeof(struct membuf ));
	if( mems == NULL ) return TCPSTRUCT_ENOMEM;
	bzero( mems , mesize * sizeof( struct membuf ));

    tcpcon = ( struct connection *) calloc( 1, MAXTCPCONNECTION * sizeof( struct connection ));
    if( tcpcon == NULL ){
        free( mems );
        return TCPSTRUCT_ENOMEM;
    } else {
        int i;
        for( i=0; i<MAXTCPCONNECTION; i++){
            tcpcon[i].use = 0;
            tcpcon[i].fd = -1;
        }
    }

	return OK;
}

int MEMBUF_unregMemBuf(  int ti )
{
    mems[ti].use=0;
    mems[ti].next = -1;
    mems[ti].len = 0;
    meuse --;
    return OK;
}

int MEMBUF_findregBlankMemBuf( void  )
{
    int i;
    for( i=0; i<mesize; i++){
        me_find ++;
        if( me_find >= mesize || me_find < 0 )
			me_find = 0;
        if( mems[me_find].use == 0 ){
            mems[me_find].use = 1;
            mems[me_find].len = 0;
            mems[me_find].next = -1;
            meuse ++;
            return me_find;
        }
    }
    return TCPSTRUCT_EMBFULL;
}

int MEMBUF_findregBlankCon( void )
{
    int i;
    for( i=0; i<MAXTCPCONNECTION; i++){
        if( tcpcon[i].use != 0 ) continue;
		tcpcon[i].use = 1;
		tcpcon[i].fd = -1;

		if( (tcpcon[i].mbtop_ri = MEMBUF_findregBlankMemBuf()) < 0 )
			return TCPSTRUCT_EMBFULL;
		if( (tcpcon[i].mbtop_wi = MEMBUF_findregBlankMemBuf()) < 0 ){
			MEMBUF_unregMemBuf( tcpcon[i].mbtop_ri );
			return TCPSTRUCT_EMBFULL;
		}
		bzero( & tcpcon[i].remoteaddr , sizeof( struct sockaddr_in));
		tcpcon[i].closed_by_remote = 0;
		return i;
    }
    return TCPSTRUCT_ECFULL;
}

int MEMBUF_getFreeMem( void )
{
    return ( mesize - meuse ) * sizeof( mems[0].buf );
}


int MEMBUF_appReadBuffer(  int ti, char *data, int len)
{
    int top, nextind;
    top = tcpcon[ti].mbtop_ri;
    for(;;){
		if( (nextind = mems[top].next) == -1 ) break;
		top = nextind;
    }
    return MEMBUF_appMemBufList( top , data , len );
}

int MEMBUF_appWriteBuffer( int ti, char *data, int len )
{
    int top, nextind;
    top = tcpcon[ti].mbtop_wi;
    for(;;){
        if( (nextind = mems[top].next) == -1 ) break;
        top = nextind;
    }

    return MEMBUF_appMemBufList( top , data , len );
}

int MEMBUF_appMemBufList( int top , char *data , int len )
{
    int fr = MEMBUF_getFreeMem( );
    int rest = len;
    int data_topaddr = 0;
    
    if( len >= fr ){
		FILE *fp;
		if( (fp=fopen( "badsysinfo.txt", "a+")) != NULL ){
			fprintf( fp, "appendMemBufList() len:%d / fr:%d err !! \n", len, fr);
			fclose( fp);
		}
        return -1;
    }

	data[len] = 0;
    for(;;){
        int blanksize = sizeof( mems[0].buf ) - mems[top].len;
        int cpsize = ( rest <= blanksize ) ? rest : blanksize;
        memcpy( mems[top].buf + mems[top].len, data + data_topaddr , cpsize );
        mems[top].len += cpsize;

        if( rest <= blanksize ){
            return len;
        } else {
            int newmb;
            rest -= cpsize;
            data_topaddr += cpsize;
            if( (newmb = MEMBUF_findregBlankMemBuf() ) == TCPSTRUCT_EMBFULL ){
				FILE *fp;
				if( (fp=fopen( "badsysinfo.txt", "a+")) != NULL ){
					fprintf( fp, "find newmb == TCPSTRUCT_EMBFULL err data:%s !!\n", data);
					fclose( fp);
				}
				log( "find newmb == TCPSTRUCT_EMBFULL err data:%s !!\n", data);
			}
            mems[top].next = newmb;
            top = mems[top].next;
        }
    }

    return TCPSTRUCT_EBUG;
}

int MEMBUF_consumeMemBufList( int top, char *out, int len, int consumeflag , int copyflag )
{
    int total = 0;
    int top_store = top;
    for(;;){
        int cpsize;
        if( top == -1 ) break;
        cpsize=(mems[top].len<=(len-total))?mems[top].len:(len-total);
        if( copyflag ) memcpy( out + total , mems[top].buf , cpsize );
        total += cpsize;
        if( consumeflag ){
            mems[top].len -= cpsize;
            if( mems[top].len > 0 ){
                memmove( mems[top].buf, mems[top].buf+cpsize, sizeof(mems[top].buf)-cpsize);
            }
        }
        top = mems[top].next;
        if( total == len ){
            break;
        }
    }
    if( consumeflag ){
        top = mems[top_store].next;
        for(;;){
            if( top == -1 )break;
            if( mems[top].len == 0 ){
                int prev;
                mems[top_store].next = mems[top].next;
                prev = top;
                top = mems[top].next;
				MEMBUF_unregMemBuf(  prev);
            } else {
                top = mems[top].next;
            }
        }
    }
    return total;
}

int MEMBUF_getLineReadBuffer( int ti , char *buf, int len )
{
	int i, l; 
    int top = tcpcon[ti].mbtop_ri;
    int tci = 0 , breakflag = 0;
    for(;;){
		l = mems[top].len;
        if( top == -1 )break;
        for( i=0 ; i < l ; i++){
            if( mems[top].buf[i] == '\n' ){
                breakflag = 1;
                break;
            }
            tci ++;
        }
        if( breakflag )break;
        top = mems[top].next;
    }
    if( tci > len )
        return TCPSTRUCT_ETOOLONG;
    if( breakflag == 0 )
        return 0;
    return MEMBUF_consumeMemBufList( tcpcon[ti].mbtop_ri , buf , tci+1 , 1 , 1 );
}

int MEMBUF_readline( int ti , char *buf , int len , int kend , int kend_r )
{
    int l;
    int minus = 0;
    if( ti < 0 || ti >= MAXTCPCONNECTION || tcpcon[ti].use == 0 )
        return TCPSTRUCT_EINVCIND;
    l = MEMBUF_getLineReadBuffer( ti , buf , len );
    if( l == 0 ){
        if( tcpcon[ti].closed_by_remote ){
            return TCPSTRUCT_EREADFIN;
        } else {
            return 0;
        }
    }
    if( kend ){
        if( buf[l-1]=='\n' ) buf[l-1] = 0; minus =-1;
    }
    if( kend_r ){
        if( buf[l-1]=='\r' ) buf[l-1] = 0; minus = -1;
        if( buf[l-2]=='\r' ) buf[l-2] = 0; minus = -2;
    }
    return l + minus;
}

int MEMBUF_readline_chop( int ti , char *buf, int len )
{
    return MEMBUF_readline( ti , buf , len , 1,1);
}

int MEMBUF_writeline( int ti , char *buf , int len )
{
    if( ti < 0 || ti >= MAXTCPCONNECTION || tcpcon[ti].use == 0 )
        return TCPSTRUCT_EINVCIND;    
    return MEMBUF_appWriteBuffer( ti , buf , len );
}

//-----------------------------------------------------------------------------

int TCPIP_bindSocket( char *addr , int p , int timeout_ms)
{
	int sockets;

	//andy_log
	log("_bindSocket( %d, %d)\n", p, timeout_ms);

    select_timeout.tv_sec = timeout_ms / 1000;
    select_timeout.tv_usec = (timeout_ms - ( timeout_ms/1000)*1000)*1000;
    //socket
    if( (sockets = socket( AF_INET , SOCK_STREAM ,  0 ) ) < 0 )
		return TCPSTRUCT_ESOCK;
    //bind
    bzero( &localaddr ,sizeof( localaddr ));
    localaddr.sin_family = AF_INET;
    localaddr.sin_port = htons( p );
    if( addr ){
        localaddr.sin_addr.s_addr = inet_addr( addr );
    } else {
        localaddr.sin_addr.s_addr = htonl( INADDR_ANY );
    }
    if( bind( sockets, (struct sockaddr*) &localaddr, sizeof( localaddr )) < 0 )
		return TCPSTRUCT_EBIND;
    //listen
    if( listen( sockets , BACKLOGNUM ) < 0 )
		return TCPSTRUCT_ELISTEN;

	tpsockfd1 = sockets;
    return OK;
}

int TCPIP_connect( char *addr , int port )
{
    int newti ;
    int s, r;
    struct sockaddr_in svaddr;
    struct hostent *he;

	if( (s = socket( AF_INET, SOCK_STREAM , 0 )) < 0 ) return -2;
	bzero( &svaddr , sizeof( svaddr ));
    svaddr.sin_family = AF_INET;
    svaddr.sin_port = htons( port );

    if( inet_aton( addr, &svaddr.sin_addr ) == 0 ){
        he = gethostbyname( addr );
        if( he == NULL ){
            return TCPSTRUCT_EHOST;
        }
        memcpy( & svaddr.sin_addr.s_addr , he->h_addr ,
                sizeof( struct in_addr));
    }
    r = connect( s , ( struct sockaddr*)&svaddr,sizeof(svaddr));
    if( r < 0 ){
        return TCPSTRUCT_ECONNECT;
    }
    set_nodelay( s );
    newti = MEMBUF_findregBlankCon( );
    if( newti < 0 ){
        log( "connection FULL: newti:%d !\n", newti );
        return TCPSTRUCT_ECFULL;
    }

    tcpcon[newti].fd = s;
    memcpy( &tcpcon[newti].remoteaddr, &svaddr, sizeof( struct sockaddr_in));
    return newti;
}


int TCPIP_close( int ti )
{
    if( ti < 0 || ti >= MAXTCPCONNECTION )return TCPSTRUCT_EINVCIND;
    if( tcpcon[ti].use == 0 ){
        return TCPSTRUCT_ECLOSEAGAIN;
    }
    close( tcpcon[ti].fd );
    tcpcon[ti].use = 0;
    tcpcon[ti].fd = -1;
    MEMBUF_consumeMemBufList( tcpcon[ti].mbtop_ri , NULL, mesize * sizeof( mems[0].buf ), 1, 0 );
    MEMBUF_consumeMemBufList( tcpcon[ti].mbtop_wi , NULL, mesize * sizeof( mems[0].buf ), 1, 0 );
    MEMBUF_unregMemBuf( tcpcon[ti].mbtop_ri );
    MEMBUF_unregMemBuf( tcpcon[ti].mbtop_wi );

    tcpcon[ti].mbtop_ri = -1;
    tcpcon[ti].mbtop_wi = -1;    
    return OK;
}

int TCPIP_selectdata( void)
{
    int i;
    int sret;
    struct timeval t;

    fd_set rfds, wfds , efds;  
    FD_ZERO( & rfds );
    FD_ZERO( & wfds );
    FD_ZERO( & efds );    

    for(i=0;i<MAXTCPCONNECTION;i++){
        if( tcpcon[i].use &&
            tcpcon[i].fd >= 0 && tcpcon[i].closed_by_remote ==0 ){
            FD_SET( tcpcon[i].fd , & rfds );
            FD_SET( tcpcon[i].fd , & wfds );
            FD_SET( tcpcon[i].fd , & efds );
        }
    }
    
    t = select_timeout;
    sret = select( 1024, & rfds , (fd_set*)NULL, & efds , &t);
	if( sret > 0 ) {
		for(i=0; i<MAXTCPCONNECTION; i++){
			if( ( tcpcon[i].fd >= 0 ) && FD_ISSET( tcpcon[i].fd , &rfds ) ){
				int fr = MEMBUF_getFreeMem( );
				int rr , readsize ;
				if( fr <= 0 ) continue;
				if( fr > sizeof(tmpbuf ) ){
					readsize = sizeof( tmpbuf);
				} else {
					readsize = fr;
				}
				rr = read( tcpcon[i].fd , tmpbuf , readsize );
				if( rr <= 0 ){
					tcpcon[i].closed_by_remote = 1;
				} else {
					MEMBUF_appReadBuffer( i , tmpbuf , rr );
				}
			}
		}
	}
    /* write */
    t = select_timeout;    
    sret = select( 1024, (fd_set*)NULL, &wfds, & efds , &t);
	if( sret > 0 ) {
		for(i=0;i<MAXTCPCONNECTION;i++){
			if( ( tcpcon[i].fd >= 0 ) && FD_ISSET( tcpcon[i].fd , &wfds ) ){
				char send_buf[4096];
				int l , rr;
				memset( send_buf, 0, sizeof( send_buf));
				l = MEMBUF_consumeMemBufList( tcpcon[i].mbtop_wi ,send_buf, sizeof(send_buf),0 , 1 );
				rr = write( tcpcon[i].fd , send_buf , l );
				if( rr < 0 ){
					tcpcon[i].closed_by_remote = 1;
				} else {
					MEMBUF_consumeMemBufList( tcpcon[i].mbtop_wi , send_buf, l, 1 , 0 );
				}
			}
		}
	}
	return 1;
}

int TCPIP_accept( int *tis , int ticount )
{
	int i, accepted=0;
	fd_set rfds, wfds , efds;

    for( i=0; i<ticount; i++){
        int asret;
        struct timeval t;
        t.tv_sec =0;
        t.tv_usec =0;
        FD_ZERO( & rfds );
        FD_ZERO( & wfds );
        FD_ZERO( & efds );

        FD_SET( tpsockfd1 , & rfds );
        FD_SET( tpsockfd1 , & wfds );
        FD_SET( tpsockfd1 , & efds );
        asret = select( 1024, &rfds , &wfds , &efds, &t );

        //if( FD_ISSET( tpsockfd1 , & rfds )){
        if( (asret > 0) && FD_ISSET( tpsockfd1 , & rfds )){
            struct sockaddr_in c;
            int len , newsockfd;
            int newcon;
            bzero( &c , sizeof( c ));
            len = sizeof( c );
            fprintf( stderr, "i can accept " );
            newcon = MEMBUF_findregBlankCon( );
            if( newcon < 0 ) continue;
            newsockfd = accept( tpsockfd1, (struct sockaddr*)&c , &len );
            log( "tcpip accept: %d\n" , newsockfd );
            if( newsockfd < 0 ){
                MEMBUF_unregMemBuf( newcon );
                continue;
            }
            set_nodelay( newsockfd );
            tcpcon[newcon].fd = newsockfd;
            memcpy( &tcpcon[newcon].remoteaddr , &c ,sizeof(c));
            tis[accepted] = newcon;
            accepted ++;
        }
    }

    return accepted;
}

int TCPIP_send( int ti , char *buf , int len )
{
	return MEMBUF_writeline( ti , buf , len );
}

int TCPIP_checkCon( int ti)
{
	if( ti < 0 || ti >= MAXTCPCONNECTION) return 0;
	return tcpcon[ti].use;
}

#endif
